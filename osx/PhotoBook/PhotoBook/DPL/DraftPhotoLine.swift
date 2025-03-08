//
//  DraftPhotoLine.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 08.03.2025.
//

import SwiftUI

struct DraftPhotoLine: View
{
    @State var frameSize:CGSize
    @ObservedObject var model: DPLModel
    
    var body: some View
    {
        ScrollView(.horizontal, showsIndicators: false) {
            HStack {
                Text("Draft")
                    .frame(width:40)
                ForEach(self.model.list.indices, id: \.self) { index in
                    
                }
            }
        }
        .frame(width: frameSize.width, height:82, alignment: .leading)
        .cornerRadius(10)
        .border(Color.BorderColor, width: 1)
    }
    
    
    
}
