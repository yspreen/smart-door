//
//  DoorView.swift
//  door
//
//  Created by Nick on 8/3/23.
//

import Foundation
import SwiftUI

struct Triangle: Shape {
	func path(in rect: CGRect) -> Path {
		var path = Path()

		path.move(to: CGPoint(x: rect.minX, y: rect.minY))
		path.addLine(to: CGPoint(x: rect.minX, y: rect.maxY))
		path.addLine(to: CGPoint(x: rect.maxX, y: rect.maxY))
		path.addLine(to: CGPoint(x: rect.minX, y: rect.minY))

		return path
	}
}

struct RevTriangle: Shape {
	func path(in rect: CGRect) -> Path {
		var path = Path()

		path.move(to: CGPoint(x: rect.minX, y: rect.minY))
		path.addLine(to: CGPoint(x: rect.maxX, y: rect.minY))
		path.addLine(to: CGPoint(x: rect.minX, y: rect.maxY))
		path.addLine(to: CGPoint(x: rect.minX, y: rect.minY))

		return path
	}
}

struct DoorView: View {
	@State var opened = false
	@State var locked = false

	var body: some View {
		VStack(spacing: 0) {
			doorFrame.zIndex(1)
				.overlay {
					doorContent
				}
				.background(alignment: .trailing) {
					VStack(spacing: 0) {
						Triangle()
							.frame(height: 5)
						Rectangle()
						RevTriangle()
							.frame(height: 5)
					}
					.padding(.vertical, 1)
					.foregroundColor(.init(white: 0.9))
					.frame(width: opened ? 5 : 0)
					.offset(x: opened ? 4 : 0)
				}
				.padding(.horizontal, 32)
				.padding(.top, 32)
				.rotation3DEffect(.degrees(opened ? -15 : 0), axis: (0, 1, 0))
				.offset(x: opened ? -4 : 0)
				.scaleEffect(opened ? 1.05 : 1)
				.brightness(opened ? -0.06 : 0)
				.background(
					Rectangle()
						.fill(Color(white: 0.99).shadow(.inner(color: .black.opacity(0.3), radius: 10)))
						.cornerRadius(5)
						.padding(.bottom, -10)
						.clipped()
						.overlay {
							Rectangle()
								.fill(Color.white.shadow(.inner(
									color: Color.black.opacity(0.1), radius: 20
								)))
								.blendMode(.multiply)
								.padding(.horizontal, -500)
								.padding(.top, -500)
						}
				)
				.padding(.horizontal, 40)
				.padding(.top, 32)
				.background(.white)
			Image("floor")
				.resizable()
				.ignoresSafeArea()
				.aspectRatio(contentMode: .fill)
				.frame(height: 50)
				.overlay {
					Rectangle()
						.fill(Color.white.shadow(.inner(
							color: Color.black.opacity(0.2), radius: 20
						)))
						.blendMode(.multiply)
						.padding(.horizontal, -500)
						.padding(.bottom, -500)
				}
		}
		.overlay {
			Color.yellow
				.ignoresSafeArea()
				.opacity(0.03)
				.blendMode(.multiply)
		}
		.onTapGesture {
			withAnimation {
				opened.toggle()
			}
		}
	}

	@ViewBuilder
	var doorContent: some View {
		lockButton
		bolt
	}

	@ViewBuilder
	var lockButton: some View {
		VStack {
			Image(systemName: locked ? "lock.fill" : "lock.open.fill")
				.resizable()
				.aspectRatio(contentMode: .fit)
				.frame(height: 30)
				.offset(x: locked ? 0 : 5.4)
				.foregroundColor(Color(white: 0.7))
				.animation(.linear(duration: 0.001).delay(0.33), value: locked)
			VStack {
				RoundedRectangle(cornerRadius: 7 / 2)
					.frame(width: 7)
					.padding()
					.foregroundColor(.init(white: 0.7))
			}
			.frame(width: 80, height: 80)
			.background(Color(white: 0.9))
			.cornerRadius(80 / 2)
			.overlay {
				Circle().stroke(Color(white: 0.7), lineWidth: 0.5)
			}
			.rotationEffect(.degrees(locked ? 0 : -45))
		}
		.onTapGesture {
			withAnimation {
				locked.toggle()
			}
		}
	}

	@ViewBuilder
	var bolt: some View {
		HStack {
			Spacer()
			Rectangle()
				.foregroundColor(Color(white: 0.7))
				.frame(height: 40)
				.frame(width: locked ? 32 : 2)
				.offset(x: locked ? 32 : 2)
		}
	}

	var doorFrame: some View {
		RoundedRectangle(cornerRadius: 5)
			.foregroundColor(.white)
			.shadow(color: .black.opacity(0.35), radius: 12)
			.frame(maxWidth: .infinity)
			.frame(maxHeight: .infinity)
	}
}

struct DoorView_Previews: PreviewProvider {
	static var previews: some View {
		DoorView().preferredColorScheme(.light)
	}
}
