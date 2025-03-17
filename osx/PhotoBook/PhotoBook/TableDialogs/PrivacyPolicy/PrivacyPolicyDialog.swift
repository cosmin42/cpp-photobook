//
//  PrivacyPolicy.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 17.03.2025.
//


import SwiftUI

struct PrivacyPolicyDialog: View {
    @Binding var isPresented: Bool
    @ObservedObject var model: PrivacyPolicyModel
    
    var body: some View {
        VStack {
            Text("Privacy Policy")
                .font(.title)
                .padding()
            ScrollView {
                Text(try! AttributedString(markdown: model.text, options: AttributedString.MarkdownParsingOptions(interpretedSyntax: .inlineOnlyPreservingWhitespace)))
            }
            
            Button("Close") {
                self.isPresented = false
            }
            .background(Color.RemoveButtonBackground)
            .foregroundColor(Color.MainFontColor)
            .cornerRadius(8)
        }
        .padding()
    }
}
