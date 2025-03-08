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
    
    @State var tabViewRatio = 0.5
    @State var selectedTab: Int = 0
    
    @State private var uplModel: UnstagedPhotoLineModel = UnstagedPhotoLineModel()
    @State private var splModel: StagedPhotoLineModel = StagedPhotoLineModel()
    @State private var mediaListModel: MediaListModel = MediaListModel()
    
    @State private var collagesGridModel: CollagesGridModel
    
    @StateObject private var canvasModel: CanvasModel = CanvasModel()
    @StateObject private var collagesCommandModel: CollagesCommandModel = CollagesCommandModel()
    
    @Binding private var lutGridModel: LutGridModel
    
    @State private var dropIndex: UInt?
    
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
                    Button(action: {
                        openImportMediaBrowser()
                    }) {
                        Image(systemName: "plus")
                            .scaledToFit()
                            .frame(width: 32, height: 32)
                            .foregroundColor(Color.MainFontColor)
                            .background(Color.clear)
                    }
                    .frame(alignment: .leading)
                    .background(Color.PrimaryColor)
                    .buttonStyle(PlainButtonStyle())
                    .help("Add Folder")
                    
                    // Remove media button
                    Button(action: {
                        print("Remove media tapped")
                        mediaListModel.removeSelected()
                    }) {
                        Image(systemName: "trash")
                            .scaledToFit()
                            .frame(width: 32, height: 32)
                            .foregroundColor(Color.MainFontColor)
                            .background(Color.clear)
                    }
                    .frame(alignment: .leading)
                    .background(Color.PrimaryColor)
                    .buttonStyle(PlainButtonStyle())
                    .disabled(mediaListModel.list.isEmpty)
                    .help("Remove Group")
                    
                    
                    Button(action: {
                        self.photobook.saveProject()
                    }) {
                        Text("💾")
                    }
                    .frame(alignment: .leading)
                    .background(Color.PrimaryColor)
                    .buttonStyle(PlainButtonStyle())
                    .help("Save")
                    
                    Divider()
                        .frame(height: 32)
                        .background(Color.gray)
                    
                    Button(action: {
                        print("Preview tapped")
                    }) {
                        Image(systemName: "eye")
                            .scaledToFit()
                            .frame(width: 32, height: 32)
                            .foregroundColor(Color.MainFontColor)
                            .background(Color.clear)
                    }
                    .frame(alignment: .leading)
                    .background(Color.PrimaryColor)
                    .buttonStyle(PlainButtonStyle())
                    .disabled(collagesCommandModel.previewDisabled)
                    .help("Preview")
                    
                    Button(action: {
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
                        if let selectedCollageIndex = self.collagesGridModel.selectedIndex
                        {
                            self.photobook.createCollage(UInt32(selectedCollageIndex), images: imagesList)
                        }
                        else
                        {
                            // TODO: Show error
                            return
                        }
                    }) {
                        Image(systemName: "square.grid.2x2")
                            .scaledToFit()
                            .frame(width: 32, height: 32)
                            .foregroundColor(Color.MainFontColor)
                            .background(Color.clear)
                    }
                    .frame(alignment: .leading)
                    .background(Color.PrimaryColor)
                    .buttonStyle(PlainButtonStyle())
                    .disabled(collagesCommandModel.makeCollageDisabled)
                    .help("Make Collage")
                    
                    Divider()
                        .frame(height: 32)
                        .background(Color.gray)
                    
                    Button(action: {
                        basicTransformationModel.imageProcessingType = .Saturation
                    }) {
                        Text("𑗘")
                    }
                    .frame(alignment: .leading)
                    .background(Color.PrimaryColor)
                    .buttonStyle(PlainButtonStyle())
                    .padding(2)
                    .overlay(
                        Rectangle()
                            .stroke(basicTransformationModel.imageProcessingType == .Saturation ? Color.white : Color.clear, lineWidth: 1)
                    )
                    .disabled(splModel.selectedIndices.isEmpty && uplModel.selectedIndices.isEmpty)
                    .help("Saturation")
                    
                    Button(action: {
                        basicTransformationModel.imageProcessingType = .Brightness
                    }) {
                        Text("☼")
                    }
                    .frame(alignment: .leading)
                    .background(Color.PrimaryColor)
                    .buttonStyle(PlainButtonStyle())
                    .padding(2)
                    .overlay(
                        Rectangle()
                            .stroke(basicTransformationModel.imageProcessingType == .Brightness ? Color.white : Color.clear, lineWidth: 1)
                    )
                    .disabled(splModel.selectedIndices.isEmpty && uplModel.selectedIndices.isEmpty)
                    .help("Brightness")
                    
                    Button(action: {
                        basicTransformationModel.imageProcessingType = .Contrast
                    }) {
                        Text("◑")
                    }
                    .frame(alignment: .leading)
                    .background(Color.PrimaryColor)
                    .buttonStyle(PlainButtonStyle())
                    .padding(2)
                    .overlay(
                        Rectangle()
                            .stroke(basicTransformationModel.imageProcessingType == .Contrast ? Color.white : Color.clear, lineWidth: 1)
                    )
                    .disabled(splModel.selectedIndices.isEmpty && uplModel.selectedIndices.isEmpty)
                    .help("Contrast")
                    
                    Divider()
                        .frame(height: 32)
                        .background(Color.gray)
                    
                    Button(action: {
                        exportDialogVisible = true
                    }) {
                        Text("📖")
                    }
                    .frame(alignment: .leading)
                    .background(Color.PrimaryColor)
                    .buttonStyle(PlainButtonStyle())
                    .disabled(splModel.list.isEmpty)
                    .help("Export")
                    
                    Button(action: {
                        subscribeDialogVisible = true
                    }) {
                        Image(systemName: "bell.fill")
                            .scaledToFit()
                            .frame(width: 32, height: 32)
                            .foregroundColor(Color.MainFontColor)
                            .background(Color.clear)
                    }
                    .frame(alignment: .leading)
                    .background(Color.PrimaryColor)
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
                                .textFieldStyle(RoundedBorderTextFieldStyle())
                                .frame(width:100)
                        }
                        
                        Slider(value:selectedAdjustment, in: selectedRange, step: 0.1)
                            .frame(width: 200)
                        
                        Button(action: {
                            basicTransformationModel.reset()
                        }) {
                            Text("Clear")
                        }
                        
                    }
                    Spacer()
                    
                    Text("Beta")
                        .padding(.vertical, 2)
                        .padding(.horizontal)
                        .background(Color(red: 0xD4/0xFF, green: 0x9F/0xFF, blue: 0x6A/0xFF))
                        .foregroundColor(Color.MainFontColor)
                        .padding(.horizontal)
                }
                .frame(alignment: .leading)
                .background(Color.PrimaryColor)
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
                            
                            CollagesGrid(model: self.collagesGridModel, frameSize: geometry.size, makeCollageDisabled: $collagesCommandModel.makeCollageDisabled, previewDisabled: $collagesCommandModel.previewDisabled)
                            
                            LutGrid(frameSize: geometry.size, model: self.lutGridModel)
                        }
                        .frame(alignment:.leading)
                        .scrollIndicators(.hidden)
                        .border(Color.BorderColor, width: 1)
                    }
                    .scrollIndicators(.hidden)
                    .frame(width: geometry.size.width * tabViewRatio)
                    
                    CanvasView(model: canvasModel, basicTransformationModel: basicTransformationModel, lutsModel: lutGridModel, frameSize: geometry.size)
                }
                
                VStack {
                    ScrollView(.horizontal, showsIndicators: false) {
                        StagedPhotoLine(frameSize: geometry.size, model: splModel, canvasImage: $canvasModel.mainImage, unstagedPhotoLineModel: $uplModel, multipleSelectionEnabled: $multipleSelectionEnabled)
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
                                                
                                                self.dropIndex = self.splModel.findPredecessorIndex(at:location)

                                                for (key, value) in images {
                                                    toPaperModel.images[key] = ToPaperData(image: value, resizeType: "Fit")
                                                }

                                                toPaperModel.showDialog.toggle()

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
                    .frame(width: geometry.size.width)
                    .onChange(of: splModel.selectedIndices)
                    {
                        _ in
                        self.collagesCommandModel.previewDisabled = !self.collagesGridModel.collagePossible()
                        self.collagesCommandModel.makeCollageDisabled = self.collagesCommandModel.previewDisabled
                    }
                    
                    
                    UnstagedPhotoLine(frameSize: geometry.size, model: uplModel, canvasImage: $canvasModel.mainImage, mediaListModel: $mediaListModel, stagedPhotoLineModel: $splModel, multipleSelectionEnabled: $multipleSelectionEnabled)
                        .onChange(of: uplModel.selectedIndices)
                    {
                        _ in
                        self.collagesCommandModel.previewDisabled = !self.collagesGridModel.collagePossible()
                        self.collagesCommandModel.makeCollageDisabled = self.collagesCommandModel.previewDisabled
                    }
                }
                .frame(height: geometry.size.height * 0.3)
            }
            .onAppear()
            {
                toPaperModel.frameSize = geometry.size
                self.uplModel.onSelectedIndicesChange = { [self] selectedIndices in
                    self.basicTransformationModel.reset()
                    self.lutGridModel.selectedIndex = nil
                    self.canvasModel.processedImageInfo = ("", "")
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
                                let image = NSImage(contentsOfFile: mainImageFrontend.resources().full)
                                if let image = image, let imagePath = imagePath
                                {
                                    let lutId = self.photobook.applyLu(inMemory: image, lutIndex: UInt32(selectedIndex))
                                    if let lutId = lutId
                                    {
                                        self.canvasModel.processedImageInfo = (lutId, imagePath)
                                    }
                                }
                            }
                        }
                    }
                }
                
                self.toPaperModel.onOk = { [self] in
                    let images: [String: FrontendImage] = toPaperModel.images.mapValues { $0.image }
                    let overlapTypes: [String:String] = toPaperModel.images.mapValues { $0.resizeType }
                    let colors: [String:NSColor] = toPaperModel.images.mapValues { NSColor($0.backgroundColor) }
                    
                    self.photobook.mapImages(toSPL: images, backgroundColors: colors, overlapTypes: overlapTypes)
                    
                    toPaperModel.images.removeAll()
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
            SubscribeDialog(isPresented: $subscribeDialogVisible)
        }
        .sheet(isPresented: $errorDialogVisible)
        {
            ErrorDialog(isPresented: $errorDialogVisible, model: errorModel)
        }
        .sheet(isPresented: $saveAreYouSureModel.showDialog)
        {
            AreYouSureDialog(model: saveAreYouSureModel)
        }
        .onAppear()
        {
            NSEvent.addLocalMonitorForEvents(matching: .flagsChanged) { event in
                self.multipleSelectionEnabled = event.modifierFlags.contains(.control)
                return event
            }
            self.collagesGridModel = CollagesGridModel(splSelectedIndices: $splModel.selectedIndices, uplSelectedIndices: $uplModel.selectedIndices)
            PhotoBookApp.pushListener(listener: self)
            exportModel.onExport = {
                name, path, exportPdf, exportPdfOpt, exportJpg in
                self.photobook.exportAlbum(path, name: name, exportPdf: exportPdf, exportPdfOptimized: exportPdfOpt, exportJpg: exportJpg)
            }
            
            self.saveAreYouSureModel.title = "Save Project"
            self.saveAreYouSureModel.message = "Do you want to save the project?"
            
            self.saveAreYouSureModel.onYes = {
                self.photobook.saveProject()
            }
            self.saveAreYouSureModel.onNo = {
            }
            
            canvasModel.onLeftClick = {
                if !uplModel.selectedIndices.isEmpty
                {
                    if let index = uplModel.selectedIndices.first
                    {
                        var newIndex = 0
                        if index == 0
                        {
                            newIndex = uplModel.list.count-1
                        }
                        else
                        {
                            newIndex = index-1
                        }
                        uplModel.selectedIndices.removeAll()
                        uplModel.selectedIndices.append(newIndex)
                        self.canvasModel.mainImage = uplModel.list[newIndex]
                    }
                }
                else if !splModel.selectedIndices.isEmpty
                {
                    if let index = splModel.selectedIndices.first
                    {
                        var newIndex = 0
                        if index == 0
                        {
                            newIndex = splModel.list.count-1
                        }
                        else
                        {
                            newIndex = index-1
                        }
                        splModel.selectedIndices.removeAll()
                        splModel.selectedIndices.append(newIndex)
                        self.canvasModel.mainImage = splModel.list[newIndex]
                    }
                }
            }
            
            canvasModel.onRightClick = {
                if !uplModel.selectedIndices.isEmpty
                {
                    if let index = uplModel.selectedIndices.first
                    {
                        let newIndex = (index+1)%uplModel.list.count
                        uplModel.selectedIndices.removeAll()
                        uplModel.selectedIndices.append(newIndex)
                        self.canvasModel.mainImage = uplModel.list[newIndex]
                    }
                }
                else if !splModel.selectedIndices.isEmpty
                {
                    if let index = splModel.selectedIndices.first
                    {
                        let newIndex = (index+1)%splModel.list.count
                        splModel.selectedIndices.removeAll()
                        splModel.selectedIndices.append(newIndex)
                        self.canvasModel.mainImage = splModel.list[newIndex]
                    }
                }
                        
            }
            
            if !toOpenProjectId.isEmpty
            {
                self.photobook.loadProject(toOpenProjectId)
            }
            self.photobook.makeCollages()
            
            self.mediaListModel.list = self.photobook.projectManagementService().unstagedImagesRepo().rowList()
            self.mediaListModel.selectedItem = self.mediaListModel.list.first
            
            let thumbnailsPath = self.photobook.getThumbnailsPath()
            
            self.splModel.list = self.photobook.projectManagementService().stagedImages().images(thumbnailsPath)
        }
        .onDisappear()
        {
            self.photobook.unloadProject()
            PhotoBookApp.popListener()
        }
        .foregroundColor(Color.MainFontColor)
        .background(Color.PrimaryColor)
        .onTapGesture{
            basicTransformationModel.imageProcessingType = .None
        }
        .onChange(of: scenePhase) { newPhase in
            switch newPhase {
            case .active:
                print("Active")
            case .inactive:
                saveAreYouSureModel.showDialog = true
            case .background:
                print("Background")
            @unknown default:
                print("Unknown")
                
            }
        }
        
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
    
    func onImageUpdated(root: String, row:UInt, index:UInt)
    {
        if let selectedRowIndex = mediaListModel.list.firstIndex(where: {$0.path == root})
        {
            if (row == selectedRowIndex)
            {
                let image = self.photobook.projectManagementService().unstagedImagesRepo().image(UInt32(row), index:UInt32(index), thumbnailsPath: self.photobook.getThumbnailsPath())
                if let unwrappedImage = image
                {
                    self.uplModel.list[Int(index)] = unwrappedImage
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
        self.splModel.insert(image: image, position: self.dropIndex)
        self.photobook.projectManagementService().stagedImages().add([image], at: UInt32(self.dropIndex ?? 0))
    }
    
    func onCollageCreated(image: FrontendImage)
    {
        self.splModel.insert(image: image, position: UInt(self.splModel.list.count))
    }

    func onLutAppliedInMemory(imageId: String, image: NSImage)
    {
        if self.canvasModel.processedImageInfo.0 == imageId
        {
            self.canvasModel.pendingLUT = false
            self.canvasModel.processedImageInfo = ("", "")
            self.canvasModel.maybeProcessedImage = image
        }
    }
    
    func onError(message: String)
    {
        errorModel.description = message
        errorDialogVisible = true
    }

    private func openImportMediaBrowser() {
        let panel = NSOpenPanel()
        panel.canChooseFiles = false
        panel.canChooseDirectories = true
        panel.allowsMultipleSelection = false
        
        panel.begin { response in
            if response == .OK, let url = panel.url {
                print("Selected folder URL (macOS): \(url)")
                self.photobook.addImportFolder(url.absoluteString)
            }
            else if response == .cancel {
                print("Folder picker was cancelled")
            }
        }
    }
}
