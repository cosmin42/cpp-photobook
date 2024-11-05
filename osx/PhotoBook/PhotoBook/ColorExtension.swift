//
//  ColorExtension.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 05.11.2024.
//

import SwiftUI

extension Color {
    init?(hex: String) {
        let hex = hex.trimmingCharacters(in: .whitespacesAndNewlines).replacingOccurrences(of: "#", with: "")
        
        // Ensure hex string is the correct length
        guard hex.count == 6 || hex.count == 8 else {
            return nil
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
        
        self.init(red: red, green: green, blue: blue, opacity: alpha)
    }
    
    static let PrimaryColor = Color("#1A2224")
    static let BorderColor = Color("#09191D")
    static let ButtonBackgroundColor = Color("#374346")
    static let ButtonPointerOverWhenSelected = Color("#516165")
    static let RemoveButtonBackground = Color("#541C1C")
    static let MainFontColor = Color("#FFFFFF")
    static let CommandBarMainColor = Color("#D3D3D3")
    static let DialogTextColor = Color("#808080")
}
