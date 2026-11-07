// Render the app icon and disk image background with system fonts.
// Usage: swift macos-assets.swift OUTPUT_DIRECTORY
import AppKit

let output = URL(fileURLWithPath: CommandLine.arguments[1], isDirectory: true)
let navy = NSColor(srgbRed: 0.035, green: 0.055, blue: 0.11, alpha: 1)
let gold = NSColor(srgbRed: 0.90, green: 0.71, blue: 0.36, alpha: 1)

func text(_ value: String, in rect: NSRect, size: CGFloat,
          color: NSColor, weight: NSFont.Weight = .regular) {
    let paragraph = NSMutableParagraphStyle()
    paragraph.alignment = .center
    (value as NSString).draw(in: rect, withAttributes: [
        .font: NSFont.systemFont(ofSize: size, weight: weight),
        .foregroundColor: color,
        .paragraphStyle: paragraph,
    ])
}

func render(_ width: Int, height: Int? = nil, to url: URL, draw: () -> Void) throws {
    let bitmap = NSBitmapImageRep(
        bitmapDataPlanes: nil, pixelsWide: width, pixelsHigh: height ?? width,
        bitsPerSample: 8, samplesPerPixel: 4, hasAlpha: true,
        isPlanar: false, colorSpaceName: .deviceRGB, bytesPerRow: 0,
        bitsPerPixel: 0)!
    NSGraphicsContext.saveGraphicsState()
    NSGraphicsContext.current = NSGraphicsContext(bitmapImageRep: bitmap)
    draw()
    NSGraphicsContext.restoreGraphicsState()
    try bitmap.representation(using: .png, properties: [:])!.write(to: url)
}

try FileManager.default.createDirectory(at: output,
                                       withIntermediateDirectories: true)
try render(1024, to: output.appendingPathComponent("icon.png")) {
    let tile = NSBezierPath(roundedRect: NSRect(x: 48, y: 48, width: 928, height: 928),
                            xRadius: 208, yRadius: 208)
    navy.setFill()
    tile.fill()
    gold.setStroke()
    tile.lineWidth = 12
    tile.stroke()
    text("WC", in: NSRect(x: 96, y: 434, width: 832, height: 362),
         size: 296, color: .white, weight: .bold)
    text("I", in: NSRect(x: 96, y: 152, width: 832, height: 300),
         size: 260, color: gold, weight: .medium)
}

try render(640, height: 440, to: output.appendingPathComponent("background.png")) {
    NSColor(srgbRed: 0.96, green: 0.95, blue: 0.93, alpha: 1).setFill()
    NSRect(x: 0, y: 0, width: 640, height: 440).fill()
    text("Wing Commander", in: NSRect(x: 30, y: 358, width: 580, height: 42),
         size: 30, color: navy, weight: .semibold)
    text("→", in: NSRect(x: 276, y: 186, width: 88, height: 80),
         size: 60, color: gold)
}
