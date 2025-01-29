//
//  UPLIdentifier.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 26.01.2025.
//

import SwiftUI
import UniformTypeIdentifiers

class UPLIdentifier: NSObject, Identifiable, NSItemProviderWriting, Encodable, Decodable
{
    public var id = UUID()
    public var row:UInt?;
    public var indices:[UInt];
    
    init(row:UInt?, indices:[UInt])
    {
        self.row = row
        self.indices = indices
    }
    
    static var writableTypeIdentifiersForItemProvider: [String] {
        return [UTType.plainText.identifier]
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
