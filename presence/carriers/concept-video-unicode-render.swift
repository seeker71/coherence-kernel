// Narrow host raster carrier for concept-video-generation-10000-13.fk.
//
// Form owns the concept, language, semantic text, visual hash, identity bits,
// and verification. This carrier only asks CoreText/AppKit to turn UTF-8 into
// pixels (the local ffmpeg build has no drawtext filter). It emits six PNG
// frames; Form subsequently asks ffmpeg for a lossless video and senses the
// decoded BMP pixels itself.

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
    bytes.reserveCapacity(value.count / 2)
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

func framePath(_ pattern: String, _ frame: Int) -> String {
    return String(format: pattern, frame + 1)
}

guard CommandLine.arguments.count == 8 else {
    fail("usage: renderer PREFIX ID LENS CAPTION_HEX GLOSS_HEX BACKDROP_PATTERN FRAMES")
}

let prefix = CommandLine.arguments[1]
guard let conceptID = Int(CommandLine.arguments[2]),
      let lens = Int(CommandLine.arguments[3]),
      let frameCount = Int(CommandLine.arguments[7]),
      conceptID >= 0, conceptID < 10_000, lens >= 0, lens < 13,
      frameCount > 0, frameCount <= 60 else { fail("invalid identity or frame count") }
let caption = decodeHex(CommandLine.arguments[4])
let gloss = decodeHex(CommandLine.arguments[5])
let backdropPattern = CommandLine.arguments[6]
let semanticHash = stableHash(caption + "\n" + gloss)

let width = 640
let height = 360
let paragraph = NSMutableParagraphStyle()
paragraph.lineBreakMode = .byTruncatingTail
paragraph.alignment = .left

for frame in 0..<frameCount {
    guard let bitmap = NSBitmapImageRep(
        bitmapDataPlanes: nil,
        pixelsWide: width,
        pixelsHigh: height,
        bitsPerSample: 8,
        samplesPerPixel: 4,
        hasAlpha: true,
        isPlanar: false,
        colorSpaceName: .deviceRGB,
        bytesPerRow: 0,
        bitsPerPixel: 0
    ) else { fail("cannot allocate bitmap") }

    NSGraphicsContext.saveGraphicsState()
    guard let context = NSGraphicsContext(bitmapImageRep: bitmap) else {
        fail("cannot create graphics context")
    }
    NSGraphicsContext.current = context
    context.imageInterpolation = .high

    let canvas = NSRect(x: 0, y: 0, width: width, height: height)
    if backdropPattern != "-", let image = NSImage(contentsOfFile: framePath(backdropPattern, frame)) {
        image.draw(in: canvas, from: .zero, operation: .sourceOver, fraction: 1.0)
    } else {
        let hueA = CGFloat((semanticHash + frame * 7) % 360) / 360.0
        let hueB = CGFloat((semanticHash / 11 + 73) % 360) / 360.0
        let gradient = NSGradient(
            starting: NSColor(calibratedHue: hueA, saturation: 0.52, brightness: 0.46, alpha: 1),
            ending: NSColor(calibratedHue: hueB, saturation: 0.72, brightness: 0.16, alpha: 1)
        )!
        gradient.draw(in: canvas, angle: CGFloat(25 + (semanticHash % 110)))
    }

    // Meaning-driven motion is deliberately visible, not metadata: both the
    // semantic hash and time move a translucent focus mark through the scene.
    let motionX = CGFloat(20 + ((semanticHash % 430) + frame * (17 + semanticHash % 13)) % 520)
    let motionY = CGFloat(92 + (semanticHash / 17) % 70)
    NSColor(calibratedWhite: 1.0, alpha: 0.22).setFill()
    NSBezierPath(ovalIn: NSRect(x: motionX, y: motionY, width: 92, height: 92)).fill()

    NSColor(calibratedWhite: 0.02, alpha: 0.78).setFill()
    NSBezierPath(roundedRect: NSRect(x: 20, y: 104, width: 600, height: 232), xRadius: 14, yRadius: 14).fill()

    let captionAttributes: [NSAttributedString.Key: Any] = [
        .font: NSFont.systemFont(ofSize: 34, weight: .semibold),
        .foregroundColor: NSColor.white,
        .paragraphStyle: paragraph
    ]
    let glossAttributes: [NSAttributedString.Key: Any] = [
        .font: NSFont.systemFont(ofSize: 18, weight: .regular),
        .foregroundColor: NSColor(calibratedWhite: 0.9, alpha: 1),
        .paragraphStyle: paragraph
    ]
    let identityAttributes: [NSAttributedString.Key: Any] = [
        .font: NSFont.monospacedSystemFont(ofSize: 14, weight: .medium),
        .foregroundColor: NSColor(calibratedWhite: 0.8, alpha: 1)
    ]

    (caption as NSString).draw(in: NSRect(x: 42, y: 250, width: 556, height: 62), withAttributes: captionAttributes)
    (gloss as NSString).draw(in: NSRect(x: 42, y: 150, width: 556, height: 88), withAttributes: glossAttributes)
    ("concept \(conceptID) · lens \(lens) · semantic \(semanticHash)" as NSString)
        .draw(at: NSPoint(x: 42, y: 120), withAttributes: identityAttributes)

    // Exact lossless Form-sensed envelope: 14 little-endian concept bits then
    // four lens bits. It remains part of the visible image at the bottom.
    for bitIndex in 0..<18 {
        let bit = bitIndex < 14
            ? ((conceptID >> bitIndex) & 1)
            : ((lens >> (bitIndex - 14)) & 1)
        (bit == 1
            ? NSColor(calibratedRed: 0.98, green: 0.12, blue: 0.07, alpha: 1)
            : NSColor(calibratedRed: 0.04, green: 0.16, blue: 0.94, alpha: 1)).setFill()
        NSBezierPath(rect: NSRect(x: 41 + bitIndex * 31, y: 8, width: 26, height: 20)).fill()
    }

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
