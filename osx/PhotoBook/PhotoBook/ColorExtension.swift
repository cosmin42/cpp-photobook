//
//  ColorExtension.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 05.11.2024.
//

import SwiftUI

extension Color {
    static func parseColor(hex: String) -> Color {
        let hex = hex.trimmingCharacters(in: .whitespacesAndNewlines).replacingOccurrences(of: "#", with: "")
        
        // Ensure hex string is the correct length
        guard hex.count == 6 || hex.count == 8 else {
            return Color.black
        }
        
        var rgb: UInt64 = 0
        Scanner(string: hex).scanHexInt64(&rgb)
        
        let red, green, blue, alpha: Double

        if hex.count == 6 {
            red = Double((rgb >> 16) & 0xFF) / 255
            green = Double((rgb >> 8) & 0xFF) / 255
            blue = Double(rgb & 0xFF) / 255
            alpha = 1.0
        } else { // 8 characters, with alpha
            alpha = Double((rgb >> 24) & 0xFF) / 255
            red = Double((rgb >> 16) & 0xFF) / 255
            green = Double((rgb >> 8) & 0xFF) / 255
            blue = Double(rgb & 0xFF) / 255
        }
        
        return Color(red: red, green: green, blue: blue, opacity: alpha)
    }
#if os(macOS)
// Generated for SwiftUI
    static let PrimaryBackgroundColor = parseColor(hex: "#1A2224")
    static let BorderColor = parseColor(hex: "#09191D")
    static let NeutralButtonBackgroundColor = parseColor(hex: "#374346")
    static let NeutralButtonPointerOverWhenSelected = parseColor(hex: "#516165")
    static let ConfirmationButtonBackground = parseColor(hex: "#374346")
    static let RemoveButtonBackground = parseColor(hex: "#541C1C")
    static let AppBarButtonDisabled = parseColor(hex: "#696969")
    static let TextFieldBackgroundColor = parseColor(hex: "#000000")
    static let MainFontColor = parseColor(hex: "#FFFFFF")
    static let CommandBarMainColor = parseColor(hex: "#D3D3D3")
    static let DialogTextColor = parseColor(hex: "#808080")
#else
// Generated for SwiftUI
    static let PrimaryBackgroundColor = parseColor(hex: "#1A2224")
    static let BorderColor = parseColor(hex: "#09191D")
    static let NeutralButtonBackgroundColor = parseColor(hex: "#374346")
    static let NeutralButtonPointerOverWhenSelected = parseColor(hex: "#516165")
    static let ConfirmationButtonBackground = parseColor(hex: "#374346")
    static let RemoveButtonBackground = parseColor(hex: "#541C1C")
    static let AppBarButtonDisabled = parseColor(hex: "#696969")
    static let TextFieldBackgroundColor = parseColor(hex: "#000000")
    static let MainFontColor = parseColor(hex: "#FFFFFF")
    static let CommandBarMainColor = parseColor(hex: "#D3D3D3")
    static let DialogTextColor = parseColor(hex: "#808080")
#endif
}
