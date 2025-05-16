//
//  DeleteProjectDialog.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 26.12.2024.
//

import SwiftUI

struct DeleteProjectDialog: View {
    @Binding var isDeleteDialogVisible: Bool
    @Binding var projectName: String
    @Binding var photobook: Photobook
    @State var projectDeleteText: String
    var body: some View {
        VStack(spacing: 20) {
            Text("Rename Album")
                .font(.headline)
            HStack {
                Text("Type DELETE:")
                TextField("", text: $projectDeleteText)
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
            }
            HStack {
                Button("Delete")
                {
                    // TODO: Do a better validation
                    if projectDeleteText == "DELETE"
                    {
                        self.photobook.projectManagementService().remove(byName: projectName)
                    }
                    // TODO: Add delete validation
                    isDeleteDialogVisible = false
                }
                .padding(8)
                .padding(.horizontal, 8)
                .background(Color.RemoveButtonBackground)
                .cornerRadius(8)
                .padding()
            }
        }
        .frame(maxWidth: .infinity, maxHeight: .infinity)
        .padding(2)
        .background(Image("delete_project") // Name of your image in Assets
        .resizable()
        .scaledToFill()
        .clipped())
    }
}

