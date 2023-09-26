//
//  Redis.swift
//  door
//
//  Created by Nick on 8/3/23.
//

import Foundation
import SwiftUI
import NIO
import RediStack

let AUTO_LOCK_AFTER_SECONDS = 15.0

class RedisStore: ObservableObject {
	static var instance = RedisStore()

	@Published var doorState = DoorState()
}

struct DoorState: Codable {
	var open = false
	var locked = true
	var autoLockAfter = Date.distantFuture.isoString
	var lastChange = Date().isoString
}

enum RedisClient {
	static let doorId = "***REMOVED***"

	static let host = "***REMOVED***"
	static let port = ***REMOVED***
	static let username = "admin"
	static let password = "***REMOVED***"
	static var redis: RedisConnection?
	static var redisListener: RedisConnection?

	static func connect() async {
		let group = await Task.detached(priority: .background) {
			return MultiThreadedEventLoopGroup(numberOfThreads: 2)
		}.value
		await withCheckedContinuation { cont in
			connectSync(group)

			cont.resume()
		}
	}

	static func connectSync(_ group: MultiThreadedEventLoopGroup) {
		do {
			try? redis?.close().wait()
			redis = try RedisConnection.make(
				configuration: try .init(address: try .makeAddressResolvingHost(host, port: port), username: username, password: password),
				boundEventLoop: group.next()
			).wait()
			try? redisListener?.close().wait()
			redisListener = try RedisConnection.make(
				configuration: try .init(address: try .makeAddressResolvingHost(host, port: port), username: username, password: password),
				boundEventLoop: group.next()
			).wait()
		} catch {
			print(error.localizedDescription)
		}
	}

	static func get(_ key: String) async -> String? {
		await withCheckedContinuation { cont in
			guard let result = try? redis?.get(
				.init(key)
			).wait()
			else {
				return cont.resume(returning: nil)
			}
			return cont.resume(returning: result.string)
		}
	}

	@discardableResult
	static func set(_ key: String, _ val: String) async -> Bool {
		await withCheckedContinuation { cont in
			guard (try? redis?.set(
				.init(key), to: val
			).wait()) != nil
			else {
				return cont.resume(returning: false)
			}
			return cont.resume(returning: true)
		}
	}

	static func poll() async {
		if
			let string = await get("door_\(doorId)"),
			let data = string.data(using: .utf8),
			let door = try? JSONDecoder().decode(DoorState.self, from: data)
		{
			await MainActor.run {
				RedisStore.instance.doorState = door
			}
		}
	}

	static var listener: Task<Void, Never>?
	static var listenerNum = 0
	static func startListener() async {
		listenerNum += 1
		let num = listenerNum
		try? await redisListener?.subscribe(to: [
			.init("door_\(doorId)")
		]) { _, val in
			if num != listenerNum { return }
			let event = val.string
			Task { await MainActor.run {
				switch event {
				case "open":
					RedisStore.instance.doorState.open = true
				case "close":
					RedisStore.instance.doorState.open = false
				case "lock":
					RedisStore.instance.doorState.locked = true
				case "unlock":
					RedisStore.instance.doorState.locked = false
				default:
					break
				}
			} }
		}.get()
	}

	static func sync() async {
		await connect()
		await poll()
		await startListener()
	}

	static func setLocked(to locked: Bool, tryReconnect: Bool = true) async {
		let action = locked ? "lock" : "unlock"
		var state = RedisStore.instance.doorState
		state.locked = locked
		if !locked {
			state.autoLockAfter = Date().addingTimeInterval(AUTO_LOCK_AFTER_SECONDS).isoString
		} else {
			state.autoLockAfter = Date.distantFuture.isoString
		}
		state.lastChange = Date().isoString
		guard
			let data = try? JSONEncoder().encode(state),
			let string = String(data: data, encoding: .utf8)
		else {
			return
		}
		await set("door_\(doorId)", string)
		do {
			_ = try await redis?.publish(
				action, to: "door_\(doorId)"
			).get()
		} catch {
			if !tryReconnect { return }
			await connect()
			await setLocked(to: locked, tryReconnect: false)
		}
	}
}
