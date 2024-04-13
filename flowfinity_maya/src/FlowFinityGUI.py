from PySide2 import QtWidgets
from PySide2 import QtCore
from shiboken2 import wrapInstance

import maya.OpenMayaUI as omui


def maya_main_window():
    main_window_ptr = omui.MQtUtil.mainWindow()
    return wrapInstance(int(main_window_ptr), QtWidgets.QWidget)


class TestDialog(QtWidgets.QDialog):

    def __init__(self, parent=maya_main_window()):
        super(TestDialog, self).__init__(parent)

        self.setWindowTitle("Test Dialog")
        self.setMinimumWidth(200)

        self.setWindowFlag(QtCore.Qt.WindowContextHelpButtonHint, False)

        self.create_widgets()
        self.create_layouts()
        self.create_connections()

    def create_widgets(self):
        self.lineedit = QtWidgets.QLineEdit()
        self.checkbox1 = QtWidgets.QCheckBox("Checkbox1")
        self.checkbox2 = QtWidgets.QCheckBox("Checkbox2")
        self.okbtn = QtWidgets.QPushButton("Ok")
        self.cancelbtn = QtWidgets.QPushButton("Cancel")

    def create_connections(self):
        self.lineedit.textChanged.connect(self.print_hello_name)
        self.cancelbtn.clicked.connect(self.close)
        self.checkbox1.toggled.connect(self.print_is_hidden)
    
    def print_hello_name(self, name):
        name = self.lineedit.text()
        print(f"Hello {name}")

    def print_is_hidden(self, checked):
        if checked:
            print("Hidden")
        else:
            print("Not Hidden")
        

    def create_layouts(self):
        form_layout = QtWidgets.QFormLayout()
        form_layout.addRow("Name:", self.lineedit)
        form_layout.addRow("Hidden:", self.checkbox1)
        form_layout.addRow("Locked:", self.checkbox2)

        button_layout = QtWidgets.QHBoxLayout()
        button_layout.addStretch()
        button_layout.addWidget(self.okbtn)
        button_layout.addWidget(self.cancelbtn)

        main_layout = QtWidgets.QVBoxLayout(self)
        main_layout.addLayout(form_layout)
        main_layout.addLayout(button_layout)


if __name__ == "__main__":
    try:
        test_dialog.close()
        test_dialog.deleteLater()
    except:
        pass

    test_dialog = TestDialog()
    test_dialog.show()
