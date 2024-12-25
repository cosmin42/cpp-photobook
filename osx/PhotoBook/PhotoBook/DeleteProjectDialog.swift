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
                    .textFieldStyle(RoundedBorderTextFieldStyle())
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
                
                Button("Cancel")
                {
                    isDeleteDialogVisible = false
                }
            }
        }
        .padding()
    }
}

