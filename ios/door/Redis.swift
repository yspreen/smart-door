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

struct DoorState {
	var open = false
	var locked = true
}

enum RedisClient {
	static let host = "nevercommit"
	static let port = 123
	static let username = "nevercommit"
	static let password = "nevercommit"
	static let redis = Redis()

	static func connect() {
		redis.connect(host: host, port: .init(port)) {
			if let err = $0 {
				return print(err)
			}
			redis.auth(password, with: username) {
				if let err = $0 {
					return print(err)
				}
				print("ok")
			}
		}
	}

	static func sync() {
		connect()
	}
}

