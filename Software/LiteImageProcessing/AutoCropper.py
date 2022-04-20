## Written by: Phillip A. Gonzales
## Written for: SGS Stephens, INC.
## Allow for automated image cropping for Visoscan images
# Use the following to install via pip
#   python -m pip install --user --trusted-host files.pythonhosted.org --trusted-host pypi.org --trusted-host pypi.python.org <NameOfPackage>
from PIL import Image
from pathlib import Path
import PySimpleGUI as sg
import os, stat
from PySimpleGUI.PySimpleGUI import popup_animated, popup_notify

sg.theme('DarkGrey8')

filePath = sg.popup_get_folder('Select a folder',
                                title = 'Visoscan Cropping Tool',
                                history = True,
                                grab_anywhere = True,
                                keep_on_top = True)

os.chmod(filePath,stat.S_IRWXO)
if filePath == None:
    sg.popup('No file chosen')
    sg.Window.close()
else:
    paths = Path(filePath).glob('*.bmp')
    numFilesConverted = 0
    for path in paths:
        pathNstr = str(path)
        fileName = Path(pathNstr).stem
        croppedFileName = fileName + '_crp.bmp'
        rawImage = Image.open(pathNstr)
        croppedImage = rawImage.crop((0, 40, 1280, 1024))
        croppedImage.save(croppedFileName, quality=100)

        #sg.popup_notify('Crop complete for image: ' + croppedFileName,
        #                display_duration_in_ms=2000,
        #                fade_in_duration=600,
        #                alpha = 0.8)
        numFilesConverted += 1

sg.popup_scrolled('Reached end of provided images\nTotal number of images cropped: '+ str(numFilesConverted) + '\n' + r'All cropped images are saved under C:\Users\YOUR_NAME\Documents',
          title = 'Proccess COMPLETE!',
          grab_anywhere=True,
          keep_on_top=True)