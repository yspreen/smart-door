//
//  Date+iso.swift
//  door
//
//  Created by Nick on 8/3/23.
//

import Foundation

extension Date {
	static let isoFormatter = {
		let dateTimeFormatter = DateFormatter()
		dateTimeFormatter.dateFormat = "yyyy-MM-dd'T'HH:mm:ss'Z'"
		dateTimeFormatter.timeZone = TimeZone.current
		dateTimeFormatter.locale = Locale.current
		dateTimeFormatter.timeZone = TimeZone(secondsFromGMT: 0)
		dateTimeFormatter.locale = Locale(identifier: "en_US_POSIX")
		return dateTimeFormatter
	}()

	var isoString: String {
		let nano = (Calendar.current.dateComponents(.init([
			.nanosecond
		]), from: self).nanosecond ?? 0) + 500
		return Date.isoFormatter.string(from: self)
			.replacingOccurrences(of: "Z", with: String(format: ".%06dZ", (nano / 1000) % 1000000))
	}

	init?(isoString str: String) {
		let fullSeconds = str.replacing(/\.\d+/, with: "")
		var nano = str.replacing(/^.*?\.(\d+).*?$/, with: { $0.output.1 })
		guard let date = Self.isoFormatter.date(from: fullSeconds) else {
			return nil
		}
		if !str.contains(".") {
			self = date
			return
		}
		while nano.count < 9 {
			nano += "0"
		}
		var components = Calendar.current.dateComponents([
			.year, .month, .day, .hour, .minute, .second, .nanosecond
		], from: date)
		components.nanosecond = Int(nano)
		guard let fullDate = Calendar.current.date(from: components) else {
			return nil
		}
		self = fullDate
	}
}
