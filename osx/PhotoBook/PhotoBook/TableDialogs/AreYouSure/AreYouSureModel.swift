//
//  AreYouSureModel.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 07.03.2025.
//

import SwiftUI

class AreYouSureModel: ObservableObject
{
    @Published public var title: String = ""
    @Published public var message: String = ""
    @Published public var onYes: () -> Void = {}
    @Published public var onNo: () -> Void = {}
    @Published public var showDialog: Bool = false
}
