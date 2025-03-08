//
//  DPLModel.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 08.03.2025.
//

import SwiftUI

class DPLModel: ObservableObject
{
    @Published public var list: [FrontendImage] = []
    @Published public var selectedIndices: [Int] = []
}
