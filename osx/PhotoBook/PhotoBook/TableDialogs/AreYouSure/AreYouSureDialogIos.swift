//
//  AreYouSureIos.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 11.05.2025.
//


import SwiftUI

struct AreYouSureDialogIos: View
{
    @ObservedObject var model: AreYouSureModel
    
    var body: some View
    {
        VStack
        {
            Text(model.title)
                .font(.title)
                .padding()
            
            Text(model.message)
                .font(.body)
                .padding()
            
                Button("Yes")
                {
                    model.onYes()
                    model.showDialog = false
                }
                .padding(8)
                .padding(.horizontal, 8)
                .background(Color.PrimaryColorIPad)
                .cornerRadius(8)
                .padding()
        }
        .frame(maxWidth: .infinity, maxHeight: .infinity)
        .background(Image("areyouusure") // Name of your image in Assets
            .resizable()
            .scaledToFill()
            .clipped())
        .padding(0)
    }
}




