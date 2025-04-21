//
//  SubscribeDialog.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 01.03.2025.
//


import SwiftUI

struct SubscribeDialog: View {
    
    @Binding var isPresented: Bool
    
    @State private var email: String = ""
    @State private var showInvalidEmailAlert: Bool = false
    
    let onSubscribe: (String) -> Void
    
    var body: some View {
        VStack {
            Text("Subscribe")
                .font(.title)
            Spacer().padding(1)
            Text("Subscribe to PB Noir newsletter to get the latest updates.")
            Spacer().padding(1)
            HStack{
                Text("Email:")
                TextField("Email", text: $email)
                    .textFieldStyle(RoundedBorderTextFieldStyle())
                    .frame(width: 200)
                Button("Subscribe") {
                    let validEmail = isValidEmail(email)
                    showInvalidEmailAlert = !validEmail
                    if validEmail
                    {
                        onSubscribe(email)
                        self.isPresented = false
                    }
                }
            }
            if showInvalidEmailAlert {
                Label("The email is not valid.", systemImage: "exclamationmark.triangle")
                    .foregroundColor(.gray)
            }
            Button("Cancel") {
                self.isPresented = false
            }
            .background(Color.RemoveButtonBackground)
            .foregroundColor(Color.MainFontColor)
        }
        .padding()
    }
    
    func isValidEmail(_ email: String) -> Bool {
        let emailFormat = "(?:[\\p{L}0-9!#$%\\&'*+/=?\\^_`{|}~-]+(?:\\.[\\p{L}0-9!#$%\\&'*+/=?\\^_`{|}" + "~-]+)*|\"(?:[\\x01-\\x08\\x0b\\x0c\\x0e-\\x1f\\x21\\x23-\\x5b\\x5d-\\" + "x7f]|\\\\[\\x01-\\x09\\x0b\\x0c\\x0e-\\x7f])*\")@(?:(?:[\\p{L}0-9](?:[a-" + "z0-9-]*[\\p{L}0-9])?\\.)+[\\p{L}0-9](?:[\\p{L}0-9-]*[\\p{L}0-9])?|\\[(?:(?:25[0-5" + "]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-" + "9][0-9]?|[\\p{L}0-9-]*[\\p{L}0-9]:(?:[\\x01-\\x08\\x0b\\x0c\\x0e-\\x1f\\x21" + "-\\x5a\\x53-\\x7f]|\\\\[\\x01-\\x09\\x0b\\x0c\\x0e-\\x7f])+)\\])";
        let emailPredicate = NSPredicate(format:"SELF MATCHES %@", emailFormat)
        return emailPredicate.evaluate(with: email)
    }
}



