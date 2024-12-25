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
                    .textFieldStyle(RoundedBorderTextFieldStyle())
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
                
                Button("Cancel")
                {
                    isRenameDialogVisible = false
                }
            }
        }
        .padding()
    }
}
