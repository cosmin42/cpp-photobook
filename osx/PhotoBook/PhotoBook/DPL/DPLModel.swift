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
    @Published public var itemFrames: [CGRect] = []
    
    public func insert(image: FrontendImage, position:UInt?)
    {
        if let position = position
        {
            self.list.insert(image, at: Int(position))
        }
        else
        {
            self.list.append(image)
        }
    }
    
    public func findPredecessorIndex(at location: CGPoint) -> UInt?
    {
        if itemFrames.isEmpty
        {
            return nil;
        }
        for i in 0..<itemFrames.count
        {
            if location.x < (itemFrames[i].origin.x + (itemFrames[i].width/2))
            {
                return UInt(i)
            }
        }
        return nil
    }
}
