//
//  ExportModel.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 01.03.2025.
//


import SwiftUI

class ExportModel: ObservableObject
{
    @Published public var albumName: String = ""
    @Published public var exportPath:String = ""
    @Published public var exportPdf: Bool = true
    @Published public var exportPdfOptimized: Bool = false
    @Published public var exportJpg: Bool = false
    @Published public var onExport: (String, String, Bool, Bool, Bool) -> Void = {_,_,_,_,_ in }
}
