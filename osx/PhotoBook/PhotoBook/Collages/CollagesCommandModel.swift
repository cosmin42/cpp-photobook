//
//  CollagesCommandModel.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 02.02.2025.
//

import SwiftUI

class CollagesCommandModel: ObservableObject
{
    @Published public var previewDisabled: Bool = true
    @Published public var makeCollageDisabled: Bool = true
}
