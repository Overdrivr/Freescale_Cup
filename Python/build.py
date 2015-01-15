from cx_Freeze import setup, Executable as cxExecutable
import platform

if platform.system() == 'Windows':
    # base must be set on Windows to either console or gui app
    # testpubsub is currently a console application
    # base = 'Win32GUI'
    base = 'Console'
else:
    base = None

opts = { 'compressed' : True,
         'create_shared_zip' : False,
         'packages' : ['pubsub.core.kwargs',
                       'pubsub.core.arg1',
                       'pubsub.core.publisherbase',
                       'pubsub.core.listenerbase',
                       'tkinter.filedialog'],
         'silent' : True
         }

WIN_Target = cxExecutable(
    script='GUI.py',
    base=base,
    targetName='GUI.exe',
    compress=True,
    appendScriptToLibrary=False,
    appendScriptToExe=True
    )

setup(
    name='Gui',
    description="FSL_CUP",
    version='0.1',
    
    options={'build_exe' : opts},
    executables=[WIN_Target]
    )
