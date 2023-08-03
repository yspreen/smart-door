//
//  Redis.swift
//  door
//
//  Created by Nick on 8/3/23.
//

import Foundation
import SwiftUI
import SwiftRedis

class RedisStore: ObservableObject {
	static var instance = RedisStore()

	@Published var doorState = DoorState()
}

struct DoorState: Codable {
	var open = false
	var locked = true
}

enum RedisClient {
	static let host = "nevercommit"
	static let port = 123
	static let username = "nevercommit"
	static let password = "nevercommit"
	static let redis = Redis()

	static func connect() async {
		await withCheckedContinuation { cont in
			connectCallback {
				cont.resume()
			}
		}
	}

	static func connectCallback(callback: () -> Void) {
		redis.connect(host: host, port: .init(port)) {
			if let err = $0 {
				return print(err)
			}
			redis.auth(password, with: username) {
				if let err = $0 {
					return print(err)
				}
				callback()
			}
		}
	}

	static func get(_ key: String) async -> String? {
		await withCheckedContinuation { cont in
			redis.get(key) { res, err in
				if let err {
					print(err)
					return cont.resume(returning: nil)
				}
				return cont.resume(returning: res?.asString)
			}
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
		startListener()
	}

	static var listener: Task<Void, Never>?
	static func startListener() {
		listener?.cancel()
		listener = Task {
			//todo
		}
	}

	static func sync() async {
		await connect()
		await poll()
	}
}
