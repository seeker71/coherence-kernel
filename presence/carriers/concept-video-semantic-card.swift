// Content-only video carrier for concept-video-open-runtime-live.fk.
//
// Form supplies UTF-8 semantic text as hexadecimal. This carrier rasterizes
// that text and motion into frames. It receives no concept id or lens id and
// emits no barcode, filename label, hidden metadata, or identity envelope.

import AppKit
import Foundation

func fail(_ message: String) -> Never {
    FileHandle.standardError.write(Data((message + "\n").utf8))
    exit(2)
}

func decodeHex(_ value: String) -> String {
    if value == "-" { return "" }
    if value.count % 2 != 0 { fail("odd UTF-8 hex payload") }
    var bytes: [UInt8] = []
    var index = value.startIndex
    while index < value.endIndex {
        let next = value.index(index, offsetBy: 2)
        guard let byte = UInt8(value[index..<next], radix: 16) else {
            fail("invalid UTF-8 hex payload")
        }
        bytes.append(byte)
        index = next
    }
    return String(decoding: bytes, as: UTF8.self)
}

func stableHash(_ value: String) -> Int {
    var hash = 17
    for byte in value.utf8 { hash = (hash * 131 + Int(byte)) % 1_000_003 }
    return hash
}

guard CommandLine.arguments.count == 6 else {
    fail("usage: semantic-card PREFIX SURFACE_HEX ANCHOR_HEX GLOSS_HEX FRAMES")
}

let prefix = CommandLine.arguments[1]
let surface = decodeHex(CommandLine.arguments[2])
let anchor = decodeHex(CommandLine.arguments[3])
let gloss = decodeHex(CommandLine.arguments[4])
guard let frameCount = Int(CommandLine.arguments[5]),
      !surface.isEmpty, frameCount > 0, frameCount <= 60 else {
    fail("invalid surface or frame count")
}

let width = 960
let height = 540
let semanticHash = stableHash(surface + "\n" + anchor + "\n" + gloss)
let titleSize: CGFloat = surface.count > 28 ? 54 : (surface.count > 14 ? 72 : 96)
let paragraph = NSMutableParagraphStyle()
paragraph.alignment = .center
paragraph.lineBreakMode = .byWordWrapping

for frame in 0..<frameCount {
    guard let bitmap = NSBitmapImageRep(
        bitmapDataPlanes: nil, pixelsWide: width, pixelsHigh: height,
        bitsPerSample: 8, samplesPerPixel: 4, hasAlpha: true,
        isPlanar: false, colorSpaceName: .deviceRGB,
        bytesPerRow: 0, bitsPerPixel: 0
    ) else { fail("cannot allocate bitmap") }

    NSGraphicsContext.saveGraphicsState()
    guard let context = NSGraphicsContext(bitmapImageRep: bitmap) else {
        fail("cannot create graphics context")
    }
    NSGraphicsContext.current = context
    context.imageInterpolation = .high

    let canvas = NSRect(x: 0, y: 0, width: width, height: height)
    let hueA = CGFloat((semanticHash + frame * 5) % 360) / 360.0
    let hueB = CGFloat((semanticHash / 11 + 83) % 360) / 360.0
    let gradient = NSGradient(
        starting: NSColor(calibratedHue: hueA, saturation: 0.42, brightness: 0.30, alpha: 1),
        ending: NSColor(calibratedHue: hueB, saturation: 0.65, brightness: 0.10, alpha: 1)
    )!
    gradient.draw(in: canvas, angle: CGFloat(20 + semanticHash % 120))

    let pulse = CGFloat(70 + (semanticHash % 80) + frame * 9)
    let motionX = CGFloat(40 + ((semanticHash % 630) + frame * 31) % 720)
    NSColor(calibratedWhite: 1, alpha: 0.12).setFill()
    NSBezierPath(ovalIn: NSRect(x: motionX, y: 72, width: pulse, height: pulse)).fill()

    NSColor(calibratedWhite: 0.015, alpha: 0.86).setFill()
    NSBezierPath(roundedRect: NSRect(x: 40, y: 76, width: 880, height: 388),
                 xRadius: 28, yRadius: 28).fill()

    let titleAttributes: [NSAttributedString.Key: Any] = [
        .font: NSFont.systemFont(ofSize: titleSize, weight: .bold),
        .foregroundColor: NSColor.white,
        .paragraphStyle: paragraph
    ]
    let anchorAttributes: [NSAttributedString.Key: Any] = [
        .font: NSFont.systemFont(ofSize: 30, weight: .semibold),
        .foregroundColor: NSColor(calibratedWhite: 0.84, alpha: 1),
        .paragraphStyle: paragraph
    ]
    let glossAttributes: [NSAttributedString.Key: Any] = [
        .font: NSFont.systemFont(ofSize: 20, weight: .regular),
        .foregroundColor: NSColor(calibratedWhite: 0.76, alpha: 1),
        .paragraphStyle: paragraph
    ]

    (surface as NSString).draw(in: NSRect(x: 72, y: 300, width: 816, height: 132),
                               withAttributes: titleAttributes)
    if anchor != surface {
        (anchor as NSString).draw(in: NSRect(x: 72, y: 244, width: 816, height: 46),
                                  withAttributes: anchorAttributes)
    }
    (gloss as NSString).draw(in: NSRect(x: 92, y: 112, width: 776, height: 116),
                             withAttributes: glossAttributes)

    context.flushGraphics()
    NSGraphicsContext.restoreGraphicsState()
    guard let png = bitmap.representation(using: .png, properties: [:]) else {
        fail("cannot encode PNG")
    }
    do {
        try png.write(to: URL(fileURLWithPath: String(format: "%@-%03d.png", prefix, frame + 1)))
    } catch { fail("cannot write PNG: \(error)") }
}

print("1", terminator: "")
