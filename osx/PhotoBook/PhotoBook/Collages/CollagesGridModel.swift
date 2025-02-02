//
//  CollagesGridModel.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 25.01.2025.
//
import SwiftUI

class CollagesGridModel: ObservableObject
{
    @Binding var splSelectedIndices: [Int]
    @Binding var uplSelectedIndices: [Int]
    
    @Published public var images:[CollageItem] = []
    @Published public var selectedIndex: Int? = nil
    
    // TODO: Do a flexible calculation here
    @Published public var columns = [
        GridItem(.flexible()),
        GridItem(.flexible()),
        GridItem(.flexible())
    ]
    
    init(splSelectedIndices:Binding<[Int]>, uplSelectedIndices:Binding<[Int]>)
    {
        _splSelectedIndices = splSelectedIndices
        _uplSelectedIndices = uplSelectedIndices
    }

    public func collagePossible() -> Bool
    {
        if let selectedIndex = self.selectedIndex
        {
            let selectedLen = max(splSelectedIndices.count, uplSelectedIndices.count)
            let collageImagesLen = self.images[selectedIndex].imagesCount
            return (selectedLen == collageImagesLen)
        }
        else
        {
            return false
        }
    }

}
