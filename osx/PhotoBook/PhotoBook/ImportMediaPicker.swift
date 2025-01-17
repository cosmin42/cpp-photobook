//
//  ImportMediaPicker.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 17.01.2025.
//

import SwiftUI

struct ImportMediaPicker: View {
    @Binding var showImportMediaPicker:Bool;
    @State var photobook:Photobook;
    
    init(showImportMediaPicker:Binding<Bool>, photobook: Photobook)
    {
        _showImportMediaPicker = showImportMediaPicker
        self.photobook = photobook
    }

    var body: some View {
        FolderPickerWrapper(showImportMediaPicker: $showImportMediaPicker, photobook: self.photobook)
    }
}

struct FolderPickerWrapper: View {
    @Binding var showImportMediaPicker:Bool;
    @State var photobook:Photobook;
    
    init(showImportMediaPicker:Binding<Bool>, photobook: Photobook)
    {
        _showImportMediaPicker = showImportMediaPicker
        self.photobook = photobook
    }
    var body: some View {
#if os(iOS)
        FolderPickeriOS()
#elseif os(macOS)
        FolderPickermacOS(showImportMediaPicker: $showImportMediaPicker, photobook: self.photobook)
#endif
    }
}

#if os(iOS)
// MARK: - iOS Folder Picker
import UIKit

struct FolderPickeriOS: UIViewControllerRepresentable {
    
    @Binding var showImportMediaPicker:Bool;
    @State var photobook:Photobook;
    
    init(showImportMediaPicker:Binding<Bool>, photobook: Photobook)
    {
        _showImportMediaPicker = showImportMediaPicker
        self.photobook = photobook
    }
    
    func makeCoordinator() -> Coordinator {
        Coordinator()
    }
    
    func makeUIViewController(context: Context) -> UIDocumentPickerViewController {
        let picker = UIDocumentPickerViewController(forOpeningContentTypes: [.folder])
        picker.delegate = context.coordinator
        picker.allowsMultipleSelection = false
        return picker
    }
    
    func updateUIViewController(_ uiViewController: UIDocumentPickerViewController, context: Context) {}
    
    class Coordinator: NSObject, UIDocumentPickerDelegate {
        func documentPicker(_ controller: UIDocumentPickerViewController, didPickDocumentsAt urls: [URL]) {
            guard let selectedURL = urls.first else { return }
            print("Selected folder URL (iOS): \(selectedURL)")
            self.photobook.addImportFolder(selectedURL)
            showImportMediaPicker = false
        }
        
        func documentPickerWasCancelled(_ controller: UIDocumentPickerViewController) {
            print("Folder picker was cancelled (iOS)")
            showImportMediaPicker = false
        }
    }
}
#endif

#if os(macOS)
// MARK: - macOS Folder Picker
import AppKit

struct FolderPickermacOS: NSViewControllerRepresentable {
    @Binding var showImportMediaPicker:Bool;
    @State var photobook:Photobook;
    
    init(showImportMediaPicker:Binding<Bool>, photobook: Photobook)
    {
        _showImportMediaPicker = showImportMediaPicker
        self.photobook = photobook
    }
    func makeNSViewController(context: Context) -> NSViewController {
        let viewController = NSViewController()
        DispatchQueue.main.async {
            let panel = NSOpenPanel()
            panel.canChooseFiles = false
            panel.canChooseDirectories = true
            panel.allowsMultipleSelection = false
            panel.begin { response in
                if response == .OK, let url = panel.url {
                    print("Selected folder URL (macOS): \(url)")
                    self.photobook.addImportFolder(url.absoluteString)
                    showImportMediaPicker = false
                }
                else if response == .cancel {
                    print("Folder picker was cancelled")
                    showImportMediaPicker = false
                }
            }
        }
        return viewController
    }
    
    func updateNSViewController(_ nsViewController: NSViewController, context: Context) {}
}
#endif
