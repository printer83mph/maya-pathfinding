import sys

from PySide2 import QtCore
from PySide2 import QtWidgets
from PySide2 import QtGui
from shiboken2 import wrapInstance

import maya.OpenMayaUI as omui
import maya.cmds as cmds

import random

# Command to send Code from VSCode to Maya
# commandPort -name "localhost:7001" -sourceType "mel"


def maya_main_window():

    # Return the Maya main window widget as a Python object

    main_window_ptr = omui.MQtUtil.mainWindow()

    # Configure for either Python 2 or 3
    if sys.version_info.major >= 3:
        return wrapInstance(int(main_window_ptr), QtWidgets.QWidget)
    else:
        return wrapInstance(long(main_window_ptr), QtWidgets.QWidget)


# Class to hold mesh data
class MeshItem:
    def __init__(self, name, fullPath):
        self.name = name
        self.fullPath = fullPath


# Class to create a dialog window to search for an instancing mesh
class SearchNavMeshDialog(QtWidgets.QDialog):
    def __init__(self, parent=maya_main_window()):
        super(SearchNavMeshDialog, self).__init__(parent)

        self.setWindowTitle("Search Mesh")
        self.setMinimumSize(300, 80)
        self.setWindowFlag(QtCore.Qt.WindowContextHelpButtonHint, False)
        self.create_widgets()
        self.create_layout()

    # Contains QListWidget to display all meshes in the scene and registers double click event
    def create_widgets(self):
        self.results_list = QtWidgets.QListWidget(self)
        self.results_list.itemDoubleClicked.connect(self.on_item_double_clicked)

    def create_layout(self):
        self.layout = QtWidgets.QVBoxLayout(self)
        self.layout.addWidget(self.results_list)

    # It will close the dialog and set the text of the search bar to the name of the mesh
    def on_item_double_clicked(self, item):
        self.close()
        self.parent().search_bar.setText(item.text())
        self.parent().instanced_mesh = MeshItem(
            item.text(), cmds.ls(item.text(), long=True)[0]
        )


# Class to create a dialog window to select start and end locators
class LocatorSelectionDialog(QtWidgets.QDialog):
    def __init__(self, parent=maya_main_window()):
        super(LocatorSelectionDialog, self).__init__(parent)
        self.setWindowTitle("Select Start/End Locators")
        self.setMinimumSize(300, 80)
        self.setWindowFlag(QtCore.Qt.WindowContextHelpButtonHint, False)
        self.create_widgets()
        self.create_layout()
        self.populate_locators()

    # Contains QListWidget to display all locators in the scene and a confirm button
    def create_widgets(self):
        self.locator_list = QtWidgets.QListWidget(self)
        self.locator_list.setSelectionMode(QtWidgets.QAbstractItemView.MultiSelection)
        self.confirm_btn = QtWidgets.QPushButton("Confirm")
        self.confirm_btn.clicked.connect(self.confirm_selection)

    def create_layout(self):
        self.layout = QtWidgets.QVBoxLayout(self)
        self.layout.addWidget(self.locator_list)
        self.layout.addWidget(self.confirm_btn)

    # Populates the QListWidget with all locators in the scene, particularly their transform nodes instead of the shape nodes
    def populate_locators(self):
        self.locator_list.clear()
        locator_shapes = cmds.ls(type="locator", long=True)
        locator_transforms = [
            cmds.listRelatives(shape, parent=True, fullPath=True)[0][1:]
            for shape in locator_shapes
        ]
        for locator in locator_transforms:
            self.locator_list.addItem(locator)

    # Checks if exactly two locators are selected and closes the dialog otherwise, a warning is shown
    def confirm_selection(self):
        selected_items = self.locator_list.selectedItems()
        if len(selected_items) == 2:
            self.accept()
        else:
            QtWidgets.QMessageBox.warning(
                self, "Warning", "Please select exactly two locators."
            )


# Class to create a dialog window to select obstacle meshes
class MeshSelectionDialog(QtWidgets.QDialog):
    def __init__(self, parent=maya_main_window()):
        super(MeshSelectionDialog, self).__init__(parent)
        self.setWindowTitle("Select Obstacle Meshes")
        self.setMinimumSize(300, 80)
        self.setWindowFlag(QtCore.Qt.WindowContextHelpButtonHint, False)
        self.create_widgets()
        self.create_layout()
        self.populate_meshes()

    # Contains QListWidget to display all locators in the scene and a confirm button
    def create_widgets(self):
        self.mesh_items = []
        self.mesh_list = QtWidgets.QListWidget(self)
        self.mesh_list.setSelectionMode(QtWidgets.QAbstractItemView.MultiSelection)
        self.confirm_btn = QtWidgets.QPushButton("Confirm")
        self.confirm_btn.clicked.connect(self.confirm_selection)

    def create_layout(self):
        self.layout = QtWidgets.QVBoxLayout(self)
        self.layout.addWidget(self.mesh_list)
        self.layout.addWidget(self.confirm_btn)

    # Populates the QListWidget with all locators in the scene, particularly their transform nodes instead of the shape nodes
    def populate_meshes(self):
        self.mesh_list.clear()
        scene_objects = cmds.ls(type="mesh")
        for obj in scene_objects:
            self.mesh_list.addItem(obj)

    # Checks if exactly two locators are selected and closes the dialog otherwise, a warning is shown
    def confirm_selection(self):
        selected_items = self.mesh_list.selectedItems()
        if len(selected_items) > 0:
            for item in selected_items:
                mesh = MeshItem(item.text(), cmds.ls(item.text(), long=True)[0])
                self.mesh_items.append(mesh)
            self.accept()
        else:
            QtWidgets.QMessageBox.warning(
                self, "Warning", "Please select multiple meshes or close the dialog."
            )


# Class to create a collapsible box widget (taken from https://stackoverflow.com/questions/52615115/how-to-create-collapsible-box-in-pyqt)
class CollapsibleBox(QtWidgets.QWidget):
    def __init__(self, title="", parent=None):
        super(CollapsibleBox, self).__init__(parent)

        # Contains a QToolButton to toggle the box
        self.toggle_button = QtWidgets.QToolButton(
            text=title, checkable=True, checked=False
        )
        self.toggle_button.setStyleSheet("QToolButton { border: none; }")
        self.toggle_button.setToolButtonStyle(QtCore.Qt.ToolButtonTextBesideIcon)
        # Change the arrow icon when the button is checked
        self.toggle_button.setArrowType(QtCore.Qt.RightArrow)
        self.toggle_button.pressed.connect(self.on_pressed)

        # Contains a QParallelAnimationGroup to animate the box
        self.toggle_animation = QtCore.QParallelAnimationGroup(self)

        # And a QScrollArea to contain the content, which is initially hidden and has a height of 0
        self.content_area = QtWidgets.QScrollArea(maximumHeight=0, minimumHeight=0)
        self.content_area.setSizePolicy(
            QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Fixed
        )
        self.content_area.setFrameShape(QtWidgets.QFrame.NoFrame)

        # This layout contains the toggle button and the content area
        lay = QtWidgets.QVBoxLayout(self)
        lay.setSpacing(0)
        lay.setContentsMargins(0, 0, 0, 0)
        lay.addWidget(self.toggle_button)
        lay.addWidget(self.content_area)

        # Animate the content area's maximum height
        self.toggle_animation.addAnimation(
            QtCore.QPropertyAnimation(self, b"minimumHeight")
        )
        self.toggle_animation.addAnimation(
            QtCore.QPropertyAnimation(self, b"maximumHeight")
        )
        self.toggle_animation.addAnimation(
            QtCore.QPropertyAnimation(self.content_area, b"maximumHeight")
        )

    # Slot to animate the box when the button is pressed
    @QtCore.Slot()
    def on_pressed(self):
        checked = self.toggle_button.isChecked()
        self.toggle_button.setArrowType(
            QtCore.Qt.DownArrow if not checked else QtCore.Qt.RightArrow
        )
        self.toggle_animation.setDirection(
            QtCore.QAbstractAnimation.Forward
            if not checked
            else QtCore.QAbstractAnimation.Backward
        )
        self.toggle_animation.start()

    # Slot to set the content layout, must be called after the widgets are added to the box layout
    def setContentLayout(self, layout):
        lay = self.content_area.layout()
        del lay
        self.content_area.setLayout(layout)
        collapsed_height = self.sizeHint().height() - self.content_area.maximumHeight()
        content_height = layout.sizeHint().height()
        for i in range(self.toggle_animation.animationCount()):
            animation = self.toggle_animation.animationAt(i)
            animation.setDuration(500)
            animation.setStartValue(collapsed_height)
            animation.setEndValue(collapsed_height + content_height)

        content_animation = self.toggle_animation.animationAt(
            self.toggle_animation.animationCount() - 1
        )
        content_animation.setDuration(500)
        content_animation.setStartValue(0)
        content_animation.setEndValue(content_height)
