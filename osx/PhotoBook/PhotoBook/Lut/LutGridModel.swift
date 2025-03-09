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
    {
        didSet {
            onSelectedIndexChange(selectedIndex)
        }
    }
    @Published public var filterText: String = ""
    @Published public var onSelectedIndexChange: (Int?) -> Void = { _ in }
    @Published public var onApply: (Int) -> Void = { _ in }
    
    // TODO: Do a flexible calculation here
    @Published public var columns = [
        GridItem(.flexible()),
        GridItem(.flexible()),
        GridItem(.flexible()),
        GridItem(.flexible())
    ]
}
