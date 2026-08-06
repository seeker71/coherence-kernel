// Thin host carrier around Apple's pretrained text recognizer.
// It sees only decoded image pixels and an optional BCP-47 language hint.
// Semantic lookup and acceptance remain in Form.

import AppKit
import Foundation
import Vision

guard CommandLine.arguments.count == 3 else {
    FileHandle.standardError.write(Data("usage: concept-video-vision-ocr IMAGE LANGUAGE\n".utf8))
    exit(64)
}

let url = URL(fileURLWithPath: CommandLine.arguments[1])
let requestedLanguage = CommandLine.arguments[2]
guard let image = NSImage(contentsOf: url),
      let data = image.tiffRepresentation,
      let bitmap = NSBitmapImageRep(data: data),
      let cgImage = bitmap.cgImage else {
    FileHandle.standardError.write(Data("image-decode-failed\n".utf8))
    exit(65)
}

let request = VNRecognizeTextRequest()
request.recognitionLevel = .accurate
request.usesLanguageCorrection = true
request.minimumTextHeight = 0.025
if requestedLanguage != "-" {
    request.recognitionLanguages = [requestedLanguage]
}

do {
    try VNImageRequestHandler(cgImage: cgImage, options: [:]).perform([request])
    let observations = request.results ?? []
    guard !observations.isEmpty else {
        FileHandle.standardError.write(Data("no-text\n".utf8))
        exit(66)
    }
    for observation in observations {
        guard let candidate = observation.topCandidates(1).first else { continue }
        let text = candidate.string
            .replacingOccurrences(of: "\\", with: "\\\\")
            .replacingOccurrences(of: "\t", with: " ")
            .replacingOccurrences(of: "\n", with: " ")
        print("\(Int((candidate.confidence * 1_000_000).rounded()))\t\(text)")
    }
} catch {
    FileHandle.standardError.write(Data("ocr-failed: \(error)\n".utf8))
    exit(67)
}
