//
//  RenameProjectDialog.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 26.12.2024.
//

import SwiftUI

struct RenameProjectDialog: View {
    @Binding var isRenameDialogVisible: Bool
    @Binding var projectName: String
    @Binding var photobook: Photobook
    @State var oldProjectName: String = ""
    
    init(isRenameDialogVisible:Binding<Bool>, projectName:Binding<String>, photobook:Binding<Photobook>)
    {
        _isRenameDialogVisible = isRenameDialogVisible
        _projectName = projectName
        _photobook = photobook
    }
    var body: some View {
        VStack(spacing: 10) {
            Text("Rename Album")
                .font(.headline)
            HStack {
                Text("New Name:")
                TextField("Project placeholder name", text: $projectName)
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
                    .onAppear()
                    {
                        self.oldProjectName = projectName
                    }
            }
            
            HStack {
                Button("Rename")
                {
                    // TODO: Do a better validation
                    if self.projectName != ""
                    {
                        self.photobook.projectManagementService().rename(self.oldProjectName, newName: self.projectName)
                    }
                    isRenameDialogVisible = false
                }
                .padding(8)
                .padding(.horizontal, 8)
                .background(Color.ConfirmationButtonBackground)
                .cornerRadius(8)
                .padding()
            }
        }
        .frame(maxWidth: .infinity, maxHeight: .infinity)
        .padding(2)
        .background(Image("rename_project") // Name of your image in Assets
        .resizable()
        .scaledToFill()
        .clipped())
    }
}
