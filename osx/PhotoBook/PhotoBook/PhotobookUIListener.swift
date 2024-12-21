//
//  PhotobookUIListener.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 03.11.2024.
//

protocol PhotobookUIListener
{
    func onProjectRead()
    func onMetadataUpdated(focusedName: String)
}
