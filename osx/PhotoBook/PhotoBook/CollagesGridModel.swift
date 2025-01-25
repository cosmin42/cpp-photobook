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
    @Published public var images = ["image1", "image2", "image3", "image4", "image5", "image6", "image7", "image8", "image9", "image10", "image11", "image12", "image13", "image14", "image15", "image16", "image17", "image18", "image19"]
    
    @Published public var columns = [
        GridItem(.flexible()),
        GridItem(.flexible()),
        GridItem(.flexible()),
        GridItem(.flexible())
    ]
    
    init(photobook: Photobook)
    {
        _photobook = State(initialValue: photobook)
    }
}
