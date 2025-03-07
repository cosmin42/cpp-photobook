//
//  AreYouSure.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 07.03.2025.
//

import SwiftUI

struct AreYouSureDialog: View
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
            
            HStack
            {
                Button("Yes")
                {
                    model.onYes()
                    model.showDialog = false
                }
                .padding()
                
                Button("No")
                {
                    model.onNo()
                    model.showDialog = false
                }
                .padding()
            }
        }
        .padding()
    }
}




