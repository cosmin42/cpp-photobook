//
//  SPLIdentifier.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 01.02.2025.
//

import SwiftUI
import UniformTypeIdentifiers

class SPLIdentifier: NSObject, Identifiable, NSItemProviderWriting, Encodable, Decodable
{
    public var id = UUID()
    public var indices:[UInt];
    
    init(indices:[UInt])
    {
        self.indices = indices
    }
    
    static var writableTypeIdentifiersForItemProvider: [String] {
        return [UTType.splDragType.identifier]
    }
    
    func serialization() -> NSData?
    {
        do {
            let encoder = JSONEncoder()
            let dataToReturn = try encoder.encode(self)
            return dataToReturn as NSData
        } catch {
            return nil
        }
    }
    
    func loadData(withTypeIdentifier typeIdentifier: String, forItemProviderCompletionHandler completionHandler: @escaping @Sendable (Data?, (any Error)?) -> Void) -> Progress? {
        do {
            let encoder = JSONEncoder()
            let dataToReturn = try encoder.encode(self)
            completionHandler(dataToReturn, nil)
        } catch {
            completionHandler(nil, error)
        }
        return Progress(totalUnitCount: 100)
    }
}
