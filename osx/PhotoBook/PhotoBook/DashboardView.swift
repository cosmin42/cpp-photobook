//
//  DashboardView.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 31.08.2023.
//

import SwiftUI

struct DashboardView: View {
    var buttonBackgroundColor: Color
    var photobook: Photobook
    init(buttonBackgroundColor:Color, photobook: Photobook)
    {
        self.buttonBackgroundColor = buttonBackgroundColor
        self.photobook = photobook
    }
    
    //var photobook: Photobook
    var body: some View {
        VStack {
            Spacer()
            HStack {
                Spacer()
                // Button with specific size in the center
                Button(action: {

                    print("Button was tapped!")
                }) {
                    Text("+").frame(minWidth: 100, minHeight: 100)
                        .padding()
                        .background(buttonBackgroundColor)
                }
                .buttonStyle(PlainButtonStyle())
                .foregroundColor(.white)
                .cornerRadius(8)
                Spacer()
            }
            Spacer()
        }
        .padding()
        .onAppear()
        {
            self.photobook.start()
        }
    }
}

struct DashboardView_Previews: PreviewProvider {
    static var previews: some View {
        DashboardView(buttonBackgroundColor: Color(red:21.6/100, green:26.3/100, blue:27.5/100), photobook: Photobook())
    }
}
