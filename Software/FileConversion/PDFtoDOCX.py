#python -m pip install --user --trusted-host files.pythonhosted.org --trusted-host pypi.org --trusted-host pypi.python.org <NameOfPackage>
# Automation of converting PDF documents to DOCX documents for MediaLab 
# Written by: Phillip A. Gonzales

from tkinter.constants import DISABLED
import PySimpleGUI as sg
import os, sys, stat
import glob
from pathlib import Path
from PySimpleGUI.PySimpleGUI import popup_animated, popup_notify
import win32com.client

import win32com
sg.theme('DarkGrey8')

filePath = sg.popup_get_folder('Select a folder',
                                title = 'PDF -> DOCX Converter',
                                history = True,
                                grab_anywhere = True,
                                keep_on_top = True)

os.chmod(filePath,stat.S_IRWXO)
if filePath == None:
    sg.popup('No file chosen')
    sg.Window.close()
else:
    paths = Path(filePath).glob('**/*.pdf')
    numFilesConverted = 0
    for path in paths:
        pathNstr = str(path)
        # Do thing with the path
        fileName = Path(pathNstr).stem
        word = win32com.client.Dispatch("Word.Application")
        doc = word.Documents.Open(pathNstr)
        doc.SaveAs(fileName, FileFormat = 16)
        doc.Close()
        word.Quit()
        sg.popup_notify('Conversion complete for file: ' + fileName,
                        display_duration_in_ms=2000,
                        fade_in_duration=600,
                        alpha = 0.8)
        numFilesConverted += 1

sg.popup_scrolled('Reached end of provided documents\nTotal number of files converted: '+ str(numFilesConverted) + '\n' + r'All converted files are saved under C:\Users\YOUR_NAME\Documents',
          title = 'Proccess COMPLETE!',
          grab_anywhere=True,
          keep_on_top=True)