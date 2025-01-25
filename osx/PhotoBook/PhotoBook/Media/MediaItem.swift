//
//  MediaItem.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 18.01.2025.
//

import SwiftUI

struct MediaItem: Hashable, Identifiable
{
    let id = UUID()
    var path: String;
    var displayName: String;
}
