//
//  LutGridModel.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 25.01.2025.
//

import SwiftUI

class LutGridModel: ObservableObject
{
    @Published public var images:[LutItem] = []
    @Published public var selectedIndex: Int? = nil
    @Published public var filterText: String = ""
    
    // TODO: Do a flexible calculation here
    @Published public var columns = [
        GridItem(.flexible()),
        GridItem(.flexible()),
        GridItem(.flexible()),
        GridItem(.flexible())
    ]
}
