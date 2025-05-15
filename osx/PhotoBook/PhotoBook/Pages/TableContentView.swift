//
//  SwiftUIView.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 29.10.2023.
//

import SwiftUI
import UniformTypeIdentifiers
import Combine

enum SimpleImageProcessingType: String
{
    case None = "None"
    case Saturation = "Saturation"
    case Brightness = "Brightness"
    case Contrast = "Contrast"
}

struct TableContentView: View, PhotobookUIListener {
    @Environment(\.scenePhase) private var scenePhase
    
    // TODO: Organize the members below
    // Organize the models and views
    @State private var navigateToDashboard = false
    @State var photobook: Photobook
    @Binding var navigationPath: [String]
    
    @State var selectedTab: Int = 0
    
    @State private var uplModel: UnstagedPhotoLineModel = UnstagedPhotoLineModel()
    @State private var splModel: StagedPhotoLineModel = StagedPhotoLineModel()
    @State private var mediaListModel: MediaListModel = MediaListModel()
    
    @State private var collagesGridModel: CollagesGridModel
    
    @StateObject private var canvasModel: CanvasModel = CanvasModel()
    
    @Binding private var lutGridModel: LutGridModel
    
    @State private var splDropIndex: UInt?
    @State private var dplDropIndex: UInt?
    
    @State private var multipleSelectionEnabled: Bool = false
    
    @StateObject private var toPaperModel: ToPaperModel = ToPaperModel()
    
    @StateObject private var basicTransformationModel: BasicTransformationModel = BasicTransformationModel()
    
    @State private var exportDialogVisible = false
    @State private var subscribeDialogVisible = false
    @State private var errorDialogVisible = false
    
    @StateObject private var exportModel: ExportModel = ExportModel()
    @Binding private var toOpenProjectId: String
    @StateObject private var errorModel: ErrorModel = ErrorModel()
    
    @StateObject private var saveAreYouSureModel: AreYouSureModel = AreYouSureModel()
    
    @StateObject private var dplModel: DPLModel = DPLModel()
    
    @StateObject private var photoLinesModel: PhotoLinesModel = PhotoLinesModel()
    
    @State private var lutFromUpl:Set<String> = []
    @State private var lutFromDpl:[String:UInt32] = [:]
    @State private var lutFromSpl:[String:UInt32] = [:]
    
    
    @State private var effectsFromUpl:Set<String> = []
    @State private var effectsFromDpl:[String:UInt32] = [:]
    @State private var effectsFromSpl:[String:UInt32] = [:]
    
    private var numberFormatter: NumberFormatter {
        let formatter = NumberFormatter()
        formatter.numberStyle = .decimal
        formatter.minimumFractionDigits = 2
        formatter.maximumFractionDigits = 2
        return formatter
    }
    
    init(navigationPath:Binding<[String]>, toOpenProjectId:Binding<String>, lutGridModel:Binding<LutGridModel>, photobook: Photobook)
    {
        _photobook = State(initialValue: photobook)
        _navigationPath = navigationPath
        _toOpenProjectId = toOpenProjectId
        _lutGridModel = lutGridModel
        _collagesGridModel = State(initialValue: CollagesGridModel(splSelectedIndices: Binding.constant([]), uplSelectedIndices: Binding.constant([])))
    }
    
    var body: some View {
        GeometryReader { geometry in
            VStack(alignment: .leading) {
                HStack(spacing: 16) {
                    Text("Beta")
                        .padding(.horizontal)
                        .background(Color(red: 0xD4/0xFF, green: 0x9F/0xFF, blue: 0x6A/0xFF))
                        .foregroundColor(Color.BorderColor)

                    Button(action: {
                        self.photobook.saveProject()
                    }) {
                        VStack{
                            Image(systemName: "square.and.arrow.down")
                                .scaledToFit()
                                .frame(width: 32, height: 32)
                                .foregroundColor(Color.MainFontColor)
                                .background(Color.clear)
                                .padding(0)
                            Text("Save").font(.system(size: 10)).padding(0)
                        }
                    }
                    .frame(alignment: .leading)
                    .background(Color.PrimaryBackgroundColor)
                    .buttonStyle(PlainButtonStyle())
                    .help("Save")
                    
                    Button(action: {
                        print("Preview tapped")
                    }) {
                        VStack{
                            Image(systemName: "eye")
                                .scaledToFit()
                                .frame(width: 32, height: 32)
                                .foregroundColor(Color.MainFontColor)
                                .background(Color.clear)
                                .padding(0)
                            Text("View").font(.system(size: 10)).padding(0)
                        }
                    }
                    .frame(alignment: .leading)
                    .background(Color.PrimaryBackgroundColor)
                    .buttonStyle(PlainButtonStyle())
                    .help("Preview")
                    
                    Button(action: {
                        exportDialogVisible = true
                    }) {
                        VStack{
                            Image(systemName: "books.vertical.fill")
                                .scaledToFit()
                                .frame(width: 32, height: 32)
                                .foregroundColor(Color.MainFontColor)
                                .background(Color.clear)
                                .padding(0)
                            Text("Export").font(.system(size: 10)).padding(0)
                        }
                    }
                    .frame(alignment: .leading)
                    .background(Color.PrimaryBackgroundColor)
                    .buttonStyle(PlainButtonStyle())
                    .disabled(splModel.list.isEmpty)
                    .help("Export")
                    
                    Button(action: {
                        subscribeDialogVisible = true
                    }) {
                        VStack{
                            Image(systemName: "bell.fill")
                                .scaledToFit()
                                .frame(width: 32, height: 32)
                                .foregroundColor(Color.MainFontColor)
                                .background(Color.clear)
                                .padding(0)
                            Text("Subscribe").font(.system(size: 10)).padding(-2)
                        }
                    }
                    .frame(alignment: .leading)
                    .background(Color.PrimaryBackgroundColor)
                    .buttonStyle(PlainButtonStyle())
                    .help("Subscribe")
                    
                    if basicTransformationModel.imageProcessingType != .None
                    {
                        var selectedAdjustment: Binding<Double> {
                            switch basicTransformationModel.imageProcessingType {
                            case .Brightness: return $basicTransformationModel.brightnessValue
                            case .Contrast: return $basicTransformationModel.contrastValue
                            case .Saturation: return $basicTransformationModel.saturationValue
                            case .None: return $basicTransformationModel.brightnessValue
                            }
                        }
                        
                        var selectedRange: ClosedRange<Double> {
                            switch basicTransformationModel.imageProcessingType {
                            case .Brightness: return basicTransformationModel.brightnessRange
                            case .Contrast: return basicTransformationModel.contrastRange
                            case .Saturation: return basicTransformationModel.saturationRange
                            case .None: return 0...1
                            }
                        }
                        
                        HStack {
                            Text("\(basicTransformationModel.imageProcessingType.rawValue):")
                            TextField("", value: selectedAdjustment, formatter: numberFormatter)
                                .frame(width:50)
                                .textFieldStyle(PlainTextFieldStyle())
#if !os(macOS)
                                .keyboardType(.numberPad)
                                .background(Color.TextFieldBackgroundColor)
                            
#endif
                        }
                        
                        Slider(value:selectedAdjustment, in: selectedRange, step: 0.1)
                            .frame(width: 200)
                        
                        Button(action: {
                            basicTransformationModel.resetOnlyValues()
                        }) {
                            Image(systemName: "xmark.circle.fill")
                                .foregroundColor(.gray)
                                .padding(5)
                        }
                        .buttonStyle(PlainButtonStyle())
                        
                        Button(action: {
                            if self.photoLinesModel.currentPhotoLine == .Draft
                            {
                                for index in dplModel.selectedIndices
                                {
                                    let image = dplModel.list[index]
                                    let uuidStr = UUID().uuidString
                                    self.effectsFromDpl[uuidStr] = UInt32(index)
                                    self.photobook.applyEffects(inPlace: uuidStr, image: image, saturation: basicTransformationModel.saturationValue, brightness: basicTransformationModel.brightnessValue, contrast: basicTransformationModel.contrastValue)
                                }
                            }
                            else if self.photoLinesModel.currentPhotoLine == .Staged
                            {
                                for index in splModel.selectedIndices
                                {
                                    let image = splModel.list[index]
                                    let uuidStr = UUID().uuidString
                                    self.effectsFromSpl[uuidStr] = UInt32(index)
                                    self.photobook.applyEffects(inPlace: uuidStr, image: image, saturation: basicTransformationModel.saturationValue, brightness: basicTransformationModel.brightnessValue, contrast: basicTransformationModel.contrastValue)
                                }
                            }
                            else if self.photoLinesModel.currentPhotoLine == .Unstaged
                            {
                                for index in uplModel.selectedIndices
                                {
                                    let image = uplModel.list[index]
                                    let uuidStr = UUID().uuidString
                                    self.effectsFromUpl.insert(uuidStr)
                                    self.photobook.applyEffects(uuidStr, image: image, saturation: basicTransformationModel.saturationValue, brightness: basicTransformationModel.brightnessValue, contrast: basicTransformationModel.contrastValue)
                                }
                            }
                        }) {
                            Text("Apply")
                        }
                        
#if !os(macOS)
                        .padding(4)
                        .background(Color.ConfirmationButtonBackground)
                        .cornerRadius(8)
                        .buttonStyle(PlainButtonStyle())
#endif
                    }
                    
                    HStack {
                        Button("Media") { selectedTab = 0 }
                            .font(.system(size: 14))
                            .frame(height: 24)
                            .padding(4)
                            .background(selectedTab == 0 ? Color.NeutralButtonPointerOverWhenSelected : Color.clear)
                            .cornerRadius(8)
                            .buttonStyle(PlainButtonStyle())
                        Button("Collages") { selectedTab = 1 }
                            .font(.system(size: 14))
                            .frame(height: 24)
                            .padding(4)
                            .background(selectedTab == 1 ? Color.NeutralButtonPointerOverWhenSelected : Color.clear)
                            .cornerRadius(8)
                            .buttonStyle(PlainButtonStyle())
                        Button("Look Up Tables") { selectedTab = 2 }
                            .font(.system(size: 14))
                            .frame(height: 24)
                            .padding(4)
                            .background(selectedTab == 2 ? Color.NeutralButtonPointerOverWhenSelected : Color.clear)
                            .cornerRadius(8)
                            .buttonStyle(PlainButtonStyle())
                    }
                    Spacer()
                }
                .frame(alignment: .leading)
                .background(Color.PrimaryBackgroundColor)
                HStack(alignment: .top) {
                    VStack(alignment:.leading) {
                        TabView(selection: $selectedTab) {
                            // Media list
                            MediaList(frameSize: geometry.size, model: self.mediaListModel)
                                .onReceive(mediaListModel.$selectedItem)
                            {
                                newValue in
                                if let newValue = newValue {
                                    // TODO: replace this with a method
                                    if let rowIndex = mediaListModel.list.firstIndex(where: {$0.path == newValue.path})
                                    {
                                        let rowSize:UInt32 = self.photobook.projectManagementService().unstagedImagesRepo().rowSize(UInt32(rowIndex))
                                        for i in 0..<rowSize {
                                            let image = self.photobook.projectManagementService().unstagedImagesRepo().image(UInt32(rowIndex), index:i, thumbnailsPath: self.photobook.getThumbnailsPath())
                                            if let unwrappedImage = image
                                            {
                                                if i >= self.uplModel.list.count
                                                {
                                                    self.uplModel.list.append(unwrappedImage)
                                                }
                                                else
                                                {
                                                    self.uplModel.list[Int(i)] = unwrappedImage
                                                }
                                            }
                                        }
                                        if (rowSize < uplModel.list.count)
                                        {
                                            uplModel.list.removeLast(Int(UInt32(uplModel.list.count) - UInt32(rowSize)))
                                        }
                                    }
                                } else {
                                    self.uplModel.list.removeAll()
                                }
                            }
                            
                            CollagesGrid(model: self.collagesGridModel, frameSize: geometry.size)
                            
                            LutGrid(frameSize: geometry.size, model: self.lutGridModel)
                        }
                        .frame(alignment:.leading)
                        .scrollIndicators(.hidden)
#if !os(macOS)
                        .tabViewStyle(PageTabViewStyle(indexDisplayMode: .never))
#endif
                    }
                    .scrollIndicators(.hidden)
                    .frame(width: geometry.size.width * NoirConstants.GoldenRatioPercentHead)
                    .border(Color.BorderColor, width: 1)
                    .background(Color.black.mix(with: Color.BorderColor, by: 0.5))
                    
                    CanvasView(model: canvasModel,
                               basicTransformationModel: basicTransformationModel,
                               lutsModel: lutGridModel,
                               stagedPhotoLineModel: splModel,
                               unstagedPhotoLineModel: uplModel,
                               draftPhotoLineModel: dplModel,
                               frameSize: geometry.size)
                    .frame(width: geometry.size.width * NoirConstants.GoldenRatioPercentBody)
                }
                .border(Color.BorderColor, width: 1)
                
                VStack {
                    VStack {
                        StagedPhotoLine(frameSize: geometry.size, model: splModel, photoLinesModel: photoLinesModel, canvasImage: $canvasModel.mainImage, unstagedPhotoLineModel: $uplModel, multipleSelectionEnabled: $multipleSelectionEnabled)
                            .background(Color.black.mix(with: Color.BorderColor, by: 0.5))
                            .onDrop(of: [.uplDragType, .splDragType], isTargeted: nil) { providers, location in
                                guard let provider = providers.first else { return false}
                                if provider.hasItemConformingToTypeIdentifier(UTType.uplDragType.identifier) {
                                    provider.loadDataRepresentation(forTypeIdentifier: UTType.uplDragType.identifier) { data, error in
                                        if let data = data {
                                            do {
                                                let uplIdentifier = try self.decodeData(data)
                                                var images: [String: FrontendImage] = [:]
                                                for index in uplIdentifier.indices
                                                {
                                                    if let row = uplIdentifier.row
                                                    {
                                                        let image = self.photobook.projectManagementService().unstagedImagesRepo().image(UInt32(row), index:UInt32(index), thumbnailsPath: self.photobook.getThumbnailsPath())
                                                        
                                                        let uuid = UUID()
                                                        images[uuid.uuidString] = image
                                                    }
                                                    else
                                                    {
                                                        if let dpLine = self.photobook.projectManagementService().draftPhotoLine(self.photobook.getThumbnailsPath())
                                                        {
                                                            let image = dpLine[Int(index)]
                                                            let uuid = UUID()
                                                            images[uuid.uuidString] = image
                                                        }
                                                    }
                                                }
                                                if let dropIndex = self.splModel.findPredecessorIndex(at:location)
                                                {
                                                    self.splDropIndex = dropIndex
                                                }
                                                else
                                                {
                                                    self.splDropIndex = UInt(self.splModel.list.count)
                                                }
                                                
                                                for (key, value) in images {
                                                    toPaperModel.images[key] = ToPaperData(image: value, resizeType: "Fit")
                                                }
                                                if (toPaperModel.images.count > 0)
                                                {
                                                    toPaperModel.showDialog.toggle()
                                                }
                                                
                                                self.uplModel.selectedIndices.removeAll()
                                                
                                            } catch {
                                                print("Failed to decode dropped data: \(error)")
                                            }
                                        }
                                        else if let error = error {
                                            assert(false, "Unreachable code \(error)")
                                        }
                                        else
                                        {
                                            assert(false, "Unreachable code")
                                        }
                                    }
                                }
                                else if provider.hasItemConformingToTypeIdentifier(UTType.splDragType.identifier) {
                                    provider.loadDataRepresentation(forTypeIdentifier: UTType.splDragType.identifier) { data, error in
                                        if let data = data {
                                            do {
                                                let splIdentifier = try self.decodeSPLData(data)
                                                let indicesToMove = splIdentifier.indices.map { Int($0) }
                                                let maybeDropIndex = self.splModel.findPredecessorIndex(at:location)
                                                
                                                self.splModel.move(fromOffsets: IndexSet(indicesToMove), toOffset: maybeDropIndex)
                                            } catch {
                                                print("Failed to decode dropped data: \(error)")
                                            }
                                        } else if let error = error {
                                            assert(false, "Unreachable code \(error)")
                                        }
                                        else
                                        {
                                            assert(false, "Unreachable code")
                                        }
                                    }
                                }
                                
                                return true
                            }
                    }
                    .onChange(of: splModel.selectedIndices)
                    {
                        _ in
                        self.collagesGridModel.previewDisabled = !self.collagesGridModel.collagePossible()
                        self.collagesGridModel.makeCollageDisabled = self.collagesGridModel.previewDisabled
                    }
                    
                    DraftPhotoLine(frameSize: geometry.size, model: dplModel, photoLinesModel: photoLinesModel, multipleSelectionEnabled: $multipleSelectionEnabled, canvasImage: $canvasModel.mainImage)
                        .background(Color.black.mix(with: Color.BorderColor, by: 0.5))
                        .onDrop(of: [.uplDragType, .splDragType], isTargeted: nil) { providers, location in
                            guard let provider = providers.first else { return false}
                            if provider.hasItemConformingToTypeIdentifier(UTType.uplDragType.identifier) {
                                provider.loadDataRepresentation(forTypeIdentifier: UTType.uplDragType.identifier) { data, error in
                                    if let data = data {
                                        do {
                                            let uplIdentifier = try self.decodeData(data)
                                            var images: [String: FrontendImage] = [:]
                                            for index in uplIdentifier.indices
                                            {
                                                if let row = uplIdentifier.row
                                                {
                                                    let image = self.photobook.projectManagementService().unstagedImagesRepo().image(UInt32(row), index:UInt32(index), thumbnailsPath: self.photobook.getThumbnailsPath())
                                                    
                                                    let uuid = UUID()
                                                    images[uuid.uuidString] = image
                                                }
                                                else
                                                {
                                                    assert(false, "Unreachable code")
                                                }
                                            }
                                            
                                            if let dropIndex = self.dplModel.findPredecessorIndex(at:location)
                                            {
                                                self.dplDropIndex = dropIndex
                                            }
                                            else
                                            {
                                                self.dplDropIndex = UInt(self.dplModel.list.count)
                                            }
                                            
                                            
                                            var toDplList: [String:FrontendImage] = [:]
                                            for (key, value) in images {
                                                    toDplList[key] = value
                                            }
                                            
                                            self.photobook.copyImages(toDpl: toDplList)
                                            
                                            self.uplModel.selectedIndices.removeAll()
                                            
                                        } catch {
                                            print("Failed to decode dropped data: \(error)")
                                        }
                                    }
                                    else if let error = error {
                                        assert(false, "Unreachable code \(error)")
                                    }
                                    else
                                    {
                                        assert(false, "Unreachable code")
                                    }
                                }
                            }
                            else if provider.hasItemConformingToTypeIdentifier(UTType.splDragType.identifier) {
                                provider.loadDataRepresentation(forTypeIdentifier: UTType.splDragType.identifier) { data, error in
                                    if let data = data {
                                        do {
                                            let splIdentifier = try self.decodeSPLData(data)
                                            let indicesToMove = splIdentifier.indices.map { Int($0) }
                                            let maybeDropIndex = self.splModel.findPredecessorIndex(at:location)
                                            
                                            self.splModel.move(fromOffsets: IndexSet(indicesToMove), toOffset: maybeDropIndex)
                                        } catch {
                                            print("Failed to decode dropped data: \(error)")
                                        }
                                    } else if let error = error {
                                        assert(false, "Unreachable code \(error)")
                                    }
                                    else
                                    {
                                        assert(false, "Unreachable code")
                                    }
                                }
                            }
                            return true
                        }
                    
                    
                    UnstagedPhotoLine(frameSize: geometry.size, model: uplModel, photoLinesModel: photoLinesModel, canvasImage: $canvasModel.mainImage, mediaListModel: $mediaListModel, stagedPhotoLineModel: $splModel, multipleSelectionEnabled: $multipleSelectionEnabled)
                        .onChange(of: uplModel.selectedIndices)
                    {
                        _ in
                        self.collagesGridModel.previewDisabled = !self.collagesGridModel.collagePossible()
                        self.collagesGridModel.makeCollageDisabled = self.collagesGridModel.previewDisabled
                    }
                }
                .frame(height: 268)
                .border(Color.BorderColor, width: 1)
                .background(Color.black.mix(with: Color.BorderColor, by: 0.5))
            }
#if os(macOS)
            .navigationBarBackButtonHidden(true)
#endif
            .onAppear()
            {
                toPaperModel.frameSize = geometry.size
                self.uplModel.onSelectedIndicesChange = { [self] selectedIndices in
                    self.basicTransformationModel.reset()
                    self.lutGridModel.selectedIndex = nil
                    self.canvasModel.processedImageInfo = ("", "")
                    if let selectedIndex = selectedIndices.first
                    {
                        self.canvasModel.mainImage = uplModel.list[selectedIndex]
                    }
                    else
                    {
                        self.canvasModel.mainImage = nil
                    }
                    self.canvasModel.maybeProcessedImage = nil
                }
                self.lutGridModel.onSelectedIndexChange = { [self] selectedIndex in
                    if let selectedIndex = selectedIndex
                    {
                        if let mainImageFrontend = canvasModel.mainImage
                        {
                            let imagePath = mainImageFrontend.resources().full
                            if self.canvasModel.processedImageInfo.1 != imagePath
                            {
#if os(macOS)
                                let image = NSImage(contentsOfFile: mainImageFrontend.resources().full)
                                if let image = image, let imagePath = imagePath
                                {
                                    let lutId = self.photobook.applyLu(inMemory: image, lutIndex: UInt32(selectedIndex))
                                    if let lutId = lutId
                                    {
                                        self.canvasModel.processedImageInfo = (lutId, imagePath)
                                    }
                                }
#else
                                let image = UIImage(contentsOfFile: mainImageFrontend.resources().full)
                                if let image = image, let imagePath = imagePath
                                {
                                    let lutId = self.photobook.applyLu(inMemory: image, lutIndex: UInt32(selectedIndex))
                                    if let lutId = lutId
                                    {
                                        self.canvasModel.processedImageInfo = (lutId, imagePath)
                                    }
                                }
#endif
                            }
                        }
                    }
                }
                
                self.lutGridModel.onApply = { [self] lutIndex in
                    
                    if self.photoLinesModel.currentPhotoLine == .Draft
                    {
                        for index in dplModel.selectedIndices
                        {
                            let image = dplModel.list[index]
                            let uuidStr = UUID().uuidString
                            lutFromDpl[uuidStr] = UInt32(lutIndex)
                            self.photobook.applyTransformation(onDisk: uuidStr, lutIndex: UInt32(lutIndex), image: image, inplace:true)
                        }
                    }
                    else if self.photoLinesModel.currentPhotoLine == .Staged
                    {
                        for index in splModel.selectedIndices
                        {
                            let image = splModel.list[index]
                            let uuidStr = UUID().uuidString
                            self.lutFromSpl[uuidStr] = UInt32(lutIndex)
                            self.photobook.applyTransformation(onDisk: uuidStr, lutIndex: UInt32(lutIndex), image: image, inplace:true)
                        }
                    }
                    else if self.photoLinesModel.currentPhotoLine == .Unstaged
                    {
                        for index in uplModel.selectedIndices
                        {
                            let image = uplModel.list[index]
                            let uuidStr = UUID().uuidString
                            self.lutFromUpl.insert(uuidStr)
                            self.photobook.applyTransformation(onDisk: uuidStr, lutIndex: UInt32(lutIndex), image: image, inplace:false)
                        }
                    }
                }
                
                self.toPaperModel.onOk = { [self] in
#if os(macOS)
                    let images: [String: FrontendImage] = toPaperModel.images.mapValues { $0.image }
                    let overlapTypes: [String:String] = toPaperModel.images.mapValues { $0.resizeType }
                    let colors: [String:NSColor] = toPaperModel.images.mapValues { NSColor($0.backgroundColor) }
                    
                    self.photobook.mapImages(toSPL: images, backgroundColors: colors, overlapTypes: overlapTypes)
                    
                    toPaperModel.images.removeAll()
#else
                    let images: [String: FrontendImage] = toPaperModel.images.mapValues { $0.image }
                    let overlapTypes: [String:String] = toPaperModel.images.mapValues { $0.resizeType }
                    let colors: [String:UIColor] = toPaperModel.images.mapValues { UIColor($0.backgroundColor) }
                    
                    self.photobook.mapImages(toSPL: images, backgroundColors: colors, overlapTypes: overlapTypes)
                    
                    toPaperModel.images.removeAll()
#endif
                }
                
                self.photoLinesModel.onPhotoLineFocusChanged = { photoLineType in
                    switch photoLineType {
                    case .Staged:
                        self.uplModel.selectedIndices.removeAll()
                        self.dplModel.selectedIndices.removeAll()
                    case .Unstaged:
                        self.dplModel.selectedIndices.removeAll()
                        self.splModel.selectedIndices.removeAll()
                    case .Draft:
                        self.uplModel.selectedIndices.removeAll()
                        self.splModel.selectedIndices.removeAll()
                    case .None:
                        break
                    }
                }
                
                self.mediaListModel.thumbnailsLocation = photobook.getThumbnailsPath()
                
                self.mediaListModel.onImagesImported = { listOfImages in
                    photobook.addImportImages(listOfImages)
                }
                
                if toOpenProjectId.isEmpty
                {
                    print("No project has to be opened")
                }
                else
                {
                    print("Opening project \(toOpenProjectId)")
                }
            }
        }
        .sheet(isPresented: $toPaperModel.showDialog)
        {
            ToPaperDialog(model: toPaperModel)
        }
        .sheet(isPresented: $exportDialogVisible)
        {
            ExportDialog(isPresented: $exportDialogVisible, model: exportModel)
        }
        .sheet(isPresented: $subscribeDialogVisible)
        {
#if os(macOS)
            SubscribeDialog(isPresented: $subscribeDialogVisible)
            {
                email in
                if let azureApiKey = SecurityUtils.getApiKey(service: "com.photobook-noir.newsletter-api-key", account: "apikey") {
                    self.photobook.subscribe(byEmail: email, apiKey: azureApiKey)
                }
                else
                {
                    assert(false)
                }
            }
#else
            SubscribeDialogIos(isPresented: $subscribeDialogVisible)
            {
                email in
                if let azureApiKey = SecurityUtils.getApiKey(service: "com.photobook-noir.newsletter-api-key", account: "apikey") {
                    self.photobook.subscribe(byEmail: email, apiKey: azureApiKey)
                }
                else
                {
                    assert(false)
                }
            }
#endif
        }
        .sheet(isPresented: $errorDialogVisible)
        {
            ErrorDialog(isPresented: $errorDialogVisible, model: errorModel)
        }
        .sheet(isPresented: $saveAreYouSureModel.showDialog)
        {
#if os(macOS)
            AreYouSureDialog(model: saveAreYouSureModel)
#else
            AreYouSureDialogIos(model: saveAreYouSureModel)
#endif
        }
        .onAppear()
        {
#if os(macOS)
            NSEvent.addLocalMonitorForEvents(matching: .flagsChanged) { event in
                self.multipleSelectionEnabled = event.modifierFlags.contains(.control)
                return event
            }
            NSEvent.addLocalMonitorForEvents(matching: .keyUp) { event in
                if event.keyCode == 51 || event.keyCode == 117
                {
                    if !splModel.selectedIndices.isEmpty || !dplModel.selectedIndices.isEmpty
                    {
                        saveAreYouSureModel.message = "Do you want to remove the images?"
                        saveAreYouSureModel.title = "Remove Images"
                        saveAreYouSureModel.onYes = {
                            self.removeImage()
                        }
                        saveAreYouSureModel.showDialog = true
                    }
                }
                else if event.keyCode == 123
                {
                    canvasModel.onLeftClick()
                }
                else if event.keyCode == 124
                {
                    canvasModel.onRightClick()
                }
                return event
            }
#endif
            self.collagesGridModel = CollagesGridModel(splSelectedIndices: $splModel.selectedIndices, uplSelectedIndices: $uplModel.selectedIndices)
            
            collagesGridModel.onApply = { selectedCollageIndex in
                var imagesIndices:[Int] = []
                var imagesList: [FrontendImage] = []
                if !self.splModel.selectedIndices.isEmpty
                {
                    imagesIndices = self.splModel.selectedIndices
                    
                    imagesList = imagesIndices.compactMap { index in
                        guard index >= 0, index < splModel.list.count else { return nil }
                        return splModel.list[index]
                    }
                }
                else if !self.uplModel.selectedIndices.isEmpty
                {
                    imagesIndices = self.uplModel.selectedIndices
                    imagesList = imagesIndices.compactMap { index in
                        guard index >= 0, index < uplModel.list.count else { return nil }
                        return uplModel.list[index]
                    }
                }
                else
                {
                    // TODO: Show error here
                    return
                }

                self.photobook.createCollage(UInt32(selectedCollageIndex), images: imagesList)

            }
            
            PhotoBookApp.pushListener(listener: self)
            exportModel.onExport = {
                name, path, exportPdf, exportPdfOpt, exportJpg in
                self.photobook.exportAlbum(path, name: name, exportPdf: exportPdf, exportPdfOptimized: exportPdfOpt, exportJpg: exportJpg)
            }
            
            canvasModel.onLeftClick = {
                switch photoLinesModel.currentPhotoLine {
                case .Unstaged:
                    let photoLineSize = uplModel.list.count
                    if let selectedIndex = uplModel.selectedIndices.first
                    {
                        let newIndex = leftIndex(index: UInt32(selectedIndex), size: UInt32(photoLineSize))
                        uplModel.selectedIndices.removeAll()
                        uplModel.selectedIndices.append(newIndex)
                        self.canvasModel.mainImage = uplModel.list[newIndex]
                    }
                case .Staged:
                    let photoLineSize = splModel.list.count
                    if let selectedIndex = splModel.selectedIndices.first
                    {
                        let newIndex = leftIndex(index: UInt32(selectedIndex), size: UInt32(photoLineSize))
                        splModel.selectedIndices.removeAll()
                        splModel.selectedIndices.append(newIndex)
                        self.canvasModel.mainImage = splModel.list[newIndex]
                    }
                case .Draft:
                    let photoLineSize = dplModel.list.count
                    if let selectedIndex = dplModel.selectedIndices.first
                    {
                        let newIndex = leftIndex(index: UInt32(selectedIndex), size: UInt32(photoLineSize))
                        dplModel.selectedIndices.removeAll()
                        dplModel.selectedIndices.append(newIndex)
                        self.canvasModel.mainImage = dplModel.list[newIndex]
                    }
                case .None:
                    break
                }
            }
            
            canvasModel.onRightClick = {
                switch photoLinesModel.currentPhotoLine {
                case .Unstaged:
                    let photoLineSize = uplModel.list.count
                    if let selectedIndex = uplModel.selectedIndices.first
                    {
                        let newIndex = rightIndex(index: UInt32(selectedIndex), size: UInt32(photoLineSize))
                        uplModel.selectedIndices.removeAll()
                        uplModel.selectedIndices.append(newIndex)
                        self.canvasModel.mainImage = uplModel.list[newIndex]
                    }
                case .Staged:
                    let photoLineSize = splModel.list.count
                    if let selectedIndex = splModel.selectedIndices.first
                    {
                        let newIndex = rightIndex(index: UInt32(selectedIndex), size: UInt32(photoLineSize))
                        splModel.selectedIndices.removeAll()
                        splModel.selectedIndices.append(newIndex)
                        self.canvasModel.mainImage = splModel.list[newIndex]
                    }
                case .Draft:
                    let photoLineSize = dplModel.list.count
                    if let selectedIndex = dplModel.selectedIndices.first
                    {
                        let newIndex = rightIndex(index: UInt32(selectedIndex), size: UInt32(photoLineSize))
                        dplModel.selectedIndices.removeAll()
                        dplModel.selectedIndices.append(newIndex)
                        self.canvasModel.mainImage = dplModel.list[newIndex]
                    }
                case .None:
                    break
                }
            }
            
            if !toOpenProjectId.isEmpty
            {
                self.photobook.loadProject(toOpenProjectId)
            }
            self.photobook.makeCollages()
            
            self.mediaListModel.onImportImages = { folderPath in
                self.photobook.addImportFolder(folderPath)
            }
            self.mediaListModel.onRemoveImages = { imagesSetIndex in
                uplModel.selectedIndices.removeAll()
                uplModel.list.removeAll()
                self.photobook.projectManagementService().unstagedImagesRepo().removeRow(imagesSetIndex)
            }
            
            self.mediaListModel.list = self.photobook.projectManagementService().unstagedImagesRepo().rowList()
            self.mediaListModel.selectedItem = self.mediaListModel.list.first
            
            let thumbnailsPath = self.photobook.getThumbnailsPath()
            
            self.splModel.list = self.photobook.projectManagementService().stagedImages().images(thumbnailsPath)
            
#if !os(macOS)
            splModel.onRemoveImage = { selection in
                // TODO: Probably the check is not needed. There is some stupidity in this code
                if !splModel.selectedIndices.isEmpty || !dplModel.selectedIndices.isEmpty
                {
                    saveAreYouSureModel.message = "Do you want to remove the images?"
                    saveAreYouSureModel.title = "Remove Images"
                    saveAreYouSureModel.onYes = {
                        self.removeImage()
                    }
                    saveAreYouSureModel.showDialog = true
                }
            }
            
            dplModel.onRemoveImage = { selection in
                if !splModel.selectedIndices.isEmpty || !dplModel.selectedIndices.isEmpty
                {
                    saveAreYouSureModel.message = "Do you want to remove the images?"
                    saveAreYouSureModel.title = "Remove Images"
                    saveAreYouSureModel.onYes = {
                        self.removeImage()
                    }
                    saveAreYouSureModel.showDialog = true
                }
            }
#endif
        }
        .onDisappear()
        {
            self.photobook.unloadProject()
            PhotoBookApp.popListener()
        }
        .foregroundColor(Color.MainFontColor)
        .background(Color.PrimaryBackgroundColor)
        .onTapGesture{
            basicTransformationModel.imageProcessingType = .None
        }
        .onChange(of: scenePhase) { newPhase in
            switch newPhase {
            case .active:
                print("Active")
            case .inactive:
                self.saveAreYouSureModel.title = "Save Project"
                self.saveAreYouSureModel.message = "Do you want to save the project?"
                self.saveAreYouSureModel.onYes = {
                    self.photobook.saveProject()
                }
            case .background:
                print("Background")
            @unknown default:
                print("Unknown")
                
            }
        }
        .ignoresSafeArea(.keyboard)
        
    }
    func decodeData(_ data: Data) throws -> UPLIdentifier {
        let decoder = JSONDecoder()
        let decoded = try decoder.decode(UPLIdentifier.self, from: data)
        return decoded
    }
    
    func decodeSPLData(_ data: Data) throws -> SPLIdentifier {
        let decoder = JSONDecoder()
        let decoded = try decoder.decode(SPLIdentifier.self, from: data)
        return decoded
    }
    
    func onProjectRead(){
    }
    func onMetadataUpdated(focusedName: String){}
    
    func onMappingFinished(root: String, imagesCount: UInt32)
    {
        let url = URL(fileURLWithPath: root)
        self.mediaListModel.list.append(MediaItem(root, displayName: url.lastPathComponent, imagesCount: imagesCount))
        self.mediaListModel.selectedItem = self.mediaListModel.list.last
    }
    
    func onSearchingFinished(importedFolderPath: String, placeholders: [String:FrontendImage])
    {
        let url = URL(fileURLWithPath: importedFolderPath)
        self.mediaListModel.list.append(MediaItem(importedFolderPath, displayName: url.lastPathComponent, imagesCount: UInt32(placeholders.count)))
        self.mediaListModel.selectedItem = self.mediaListModel.list.last
    }
    
    func onImageUpdated(root: String, row:UInt, index:UInt)
    {
        if let selectedRowIndex = mediaListModel.list.firstIndex(where: {$0.path == root})
        {
            if (row == selectedRowIndex)
            {
                let image = self.photobook.projectManagementService().unstagedImagesRepo().image(UInt32(row), index:UInt32(index), thumbnailsPath: self.photobook.getThumbnailsPath())
                
                if row == self.mediaListModel.selectedIndex()
                {
                    if let unwrappedImage = image
                    {
                        self.uplModel.list[Int(index)] = unwrappedImage
                    }
                }
            }
        }
    }
    
    func onCollageThumbnailsCreated(){
        let collageList = self.photobook.collageTemplatesThumbnailsList()
        if let collageList = collageList
        {
            for i in 0..<collageList.count
            {
                if (self.collagesGridModel.images.count <= i)
                {
                    collagesGridModel.images.append(collageList[i])
                }
                else
                {
                    collagesGridModel.images[i] = collageList[i]
                }
            }
            if (collageList.count < collagesGridModel.images.count)
            {
                collagesGridModel.images.removeLast((collagesGridModel.images.count-collageList.count))
            }
        }
    }
    
    func onImageMapped(imageId: String, image: FrontendImage)
    {
        self.splModel.insert(image: image, position: self.splDropIndex)
        self.photobook.projectManagementService().stagedImages().add([image], at: UInt32(self.splDropIndex ?? 0))
    }
    
    func onImageCopied(imageId: String, image: FrontendImage)
    {
        self.dplModel.insert(image: image, position: self.dplDropIndex)
        self.photobook.projectManagementService().insert(image, at: UInt32(self.dplDropIndex ?? 0))
    }
    
    func onCollageCreated(image: FrontendImage)
    {
        self.dplModel.list.append(image)
        self.photobook.projectManagementService().append(_:image)
    }
    
#if os(macOS)
    func onLutAppliedInMemory(imageId: String, image: NSImage)
    {
        if self.canvasModel.processedImageInfo.0 == imageId
        {
            self.canvasModel.pendingLUT = false
            self.canvasModel.processedImageInfo = ("", "")
            self.canvasModel.maybeProcessedImage = image
        }
    }
#else
    func onLutAppliedInMemory(imageId: String, image: UIImage)
    {
        if self.canvasModel.processedImageInfo.0 == imageId
        {
            self.canvasModel.pendingLUT = false
            self.canvasModel.processedImageInfo = ("", "")
            self.canvasModel.maybeProcessedImage = image
        }
    }
#endif
    
    func onLutAppliedOnDiskInplace(imageId: String)
    {
        if self.lutFromDpl.keys.contains(imageId)
        {
            self.dplModel.list[Int(lutFromDpl[imageId]!)] = self.photobook.projectManagementService().draftPhotoLine(self.photobook.getThumbnailsPath())![Int(lutFromDpl[imageId]!)]
            lutFromDpl.removeValue(forKey: imageId)
        }
        else if self.lutFromSpl.keys.contains(imageId)
        {
            self.splModel.list[Int(lutFromSpl[imageId]!)] = self.photobook.projectManagementService().stagedImages().images(self.photobook.getThumbnailsPath())[Int(lutFromSpl[imageId]!)]
            lutFromSpl.removeValue(forKey: imageId)
        }
        
    }
    
    func onLutAppliedOnDisk(imageId: String, image: FrontendImage)
    {
        lutFromUpl.remove(imageId)
        dplModel.insert(image: image, position: self.dplDropIndex)
        self.photobook.projectManagementService().insert(image, at: UInt32(self.dplDropIndex ?? 0))
    }
    
    func onEffectsApplied(imageId: String, image: FrontendImage)
    {
        print("\(image.resources().full)")
        self.effectsFromUpl.remove(imageId)
        self.dplModel.list.append(image)
        self.photobook.projectManagementService().append(image)
    }
    
    func onEffectsAppliedInplace(imageId: String)
    {
        let imageId = imageId.uppercased()
        
        if self.effectsFromDpl.keys.contains(imageId)
        {
            self.dplModel.list[Int(effectsFromDpl[imageId]!)] = self.photobook.projectManagementService().draftPhotoLine(self.photobook.getThumbnailsPath())![Int(effectsFromDpl[imageId]!)]
            effectsFromDpl.removeValue(forKey: imageId)
        }
        else if self.effectsFromSpl.keys.contains(imageId)
        {
            self.splModel.list[Int(effectsFromSpl[imageId]!)] = self.photobook.projectManagementService().stagedImages().images(self.photobook.getThumbnailsPath())[Int(effectsFromSpl[imageId]!)]
            effectsFromSpl.removeValue(forKey: imageId)
        }
    }
    
    func onError(message: String)
    {
        errorModel.description = message
        errorDialogVisible = true
    }
    
    private func leftIndex(index: UInt32, size: UInt32) -> Int {
        if index == 0 {
            return Int(size - 1)
        }
        return Int(index - 1)
    }
    
    private func rightIndex(index: UInt32, size: UInt32) -> Int {
        return Int((index + 1) % size)
    }
    
    private func removeImage()
    {
        if !splModel.selectedIndices.isEmpty
        {
            let tmpSelectedIndices = splModel.selectedIndices
            splModel.selectedIndices.removeAll()
            splModel.list.remove(atOffsets: IndexSet(tmpSelectedIndices))
            let nsNumberSet: [NSNumber] = tmpSelectedIndices.map({ NSNumber(value:$0) })
            self.photobook.projectManagementService().stagedImages().removeImages(nsNumberSet)
        }
        else if !dplModel.selectedIndices.isEmpty
        {
            let tmpSelectedIndices = dplModel.selectedIndices
            dplModel.selectedIndices.removeAll()
            dplModel.list.remove(atOffsets: IndexSet(tmpSelectedIndices))
            let nsNumberSet: [NSNumber] = tmpSelectedIndices.map({ NSNumber(value:$0) })
            self.photobook.projectManagementService().removeDraftImages(nsNumberSet)
        }
    }
}
