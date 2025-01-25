//
//  CollagesGridModel.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 25.01.2025.
//
import SwiftUI

class CollagesGridModel: ObservableObject
{
    @State var photobook: Photobook
    @Published public var images:[CollageItem] = []
    
    // TODO: Do a flexible calculation here
    @Published public var columns = [
        GridItem(.flexible()),
        GridItem(.flexible()),
        GridItem(.flexible())
    ]
    
    init(photobook: Photobook)
    {
        _photobook = State(initialValue: photobook)
    }
}
