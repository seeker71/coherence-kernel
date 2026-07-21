import AppKit
import Foundation
import Vision

// Thin host carrier around the operating system's pretrained image classifier.
// It emits only the model's raw identifier/confidence rows. Concept mapping,
// thresholds, temporal voting, and world-model persistence remain in Form.

guard CommandLine.arguments.count == 2 else {
    FileHandle.standardError.write(Data("usage: concept-video-vision-classify IMAGE\n".utf8))
    exit(64)
}

let url = URL(fileURLWithPath: CommandLine.arguments[1])
guard let image = NSImage(contentsOf: url),
      let data = image.tiffRepresentation,
      let bitmap = NSBitmapImageRep(data: data),
      let cgImage = bitmap.cgImage else {
    FileHandle.standardError.write(Data("image-decode-failed\n".utf8))
    exit(65)
}

let request = VNClassifyImageRequest()
let handler = VNImageRequestHandler(cgImage: cgImage, options: [:])

do {
    try handler.perform([request])
    let observations = (request.results ?? []).prefix(20)
    guard !observations.isEmpty else {
        FileHandle.standardError.write(Data("no-classifications\n".utf8))
        exit(66)
    }
    for observation in observations {
        // Identifiers are escaped so every observation stays one parseable row.
        let identifier = observation.identifier
            .replacingOccurrences(of: "\\", with: "\\\\")
            .replacingOccurrences(of: "\t", with: "\\t")
            .replacingOccurrences(of: "\n", with: "\\n")
        print("\(Int((observation.confidence * 1_000_000).rounded()))\t\(identifier)")
    }
} catch {
    FileHandle.standardError.write(Data("classification-failed: \(error)\n".utf8))
    exit(67)
}
