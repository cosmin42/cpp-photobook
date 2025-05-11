//
//  SubscribeDialogIpad.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 11.05.2025.
//

import SwiftUI

struct SubscribeDialogIos: View {
    
    @Binding var isPresented: Bool
    
    @State private var email: String = ""
    @State private var showInvalidEmailAlert: Bool = false
    
    let onSubscribe: (String) -> Void
    
    var body: some View {
        VStack {
            Text("Subscribe")
                .font(.title)
                .padding()
            Text("Subscribe to PB Noir newsletter to get the latest updates.")
            HStack{
                Text("Email:")
                TextField("email here...", text: $email)
                    .frame(width: 200, height: 40)
                    .textFieldStyle(PlainTextFieldStyle())
                    .background(
                        RoundedRectangle(cornerRadius: 8)
                            .fill(Color.TextFieldBackgroundColor) // Background color
                    )
                    .overlay(
                        RoundedRectangle(cornerRadius: 8)
                            .stroke(Color.BorderColor, lineWidth: 1) // Border
                    )
                    
                Button("Subscribe") {
                    let validEmail = isValidEmail(email)
                    showInvalidEmailAlert = !validEmail
                    if validEmail
                    {
                        onSubscribe(email)
                        self.isPresented = false
                    }
                }
                .padding(8)
                .padding(.horizontal, 8)
                .background(Color.PrimaryColorIPad)
                .cornerRadius(8)
                .padding()
            }
            if showInvalidEmailAlert {
                Label("The email is not valid.", systemImage: "exclamationmark.triangle")
                    .foregroundColor(.gray)
            }
#if os(macOS)
            Button("Cancel") {
                self.isPresented = false
            }
            .padding(8)
            .padding(.horizontal, 8)
            .background(Color.gray)
            .cornerRadius(8)
            .padding()
#endif
        }
        .frame(maxWidth: .infinity, maxHeight: .infinity)
        .background(Image("subscribe_background") // Name of your image in Assets
            .resizable()
            .scaledToFill()
            .clipped())
        .padding(0)
    }
    
    func isValidEmail(_ email: String) -> Bool {
        let emailFormat = "(?:[\\p{L}0-9!#$%\\&'*+/=?\\^_`{|}~-]+(?:\\.[\\p{L}0-9!#$%\\&'*+/=?\\^_`{|}" + "~-]+)*|\"(?:[\\x01-\\x08\\x0b\\x0c\\x0e-\\x1f\\x21\\x23-\\x5b\\x5d-\\" + "x7f]|\\\\[\\x01-\\x09\\x0b\\x0c\\x0e-\\x7f])*\")@(?:(?:[\\p{L}0-9](?:[a-" + "z0-9-]*[\\p{L}0-9])?\\.)+[\\p{L}0-9](?:[\\p{L}0-9-]*[\\p{L}0-9])?|\\[(?:(?:25[0-5" + "]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-" + "9][0-9]?|[\\p{L}0-9-]*[\\p{L}0-9]:(?:[\\x01-\\x08\\x0b\\x0c\\x0e-\\x1f\\x21" + "-\\x5a\\x53-\\x7f]|\\\\[\\x01-\\x09\\x0b\\x0c\\x0e-\\x7f])+)\\])";
        let emailPredicate = NSPredicate(format:"SELF MATCHES %@", emailFormat)
        return emailPredicate.evaluate(with: email)
    }
}



