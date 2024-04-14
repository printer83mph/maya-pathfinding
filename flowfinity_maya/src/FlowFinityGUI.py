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


# Class to create a dialog window to search for a navigation mesh
class SearchNavMeshDialog(QtWidgets.QDialog):
    def __init__(self, parent=maya_main_window()):
        super(SearchNavMeshDialog, self).__init__(parent)

        self.setWindowTitle("Search NavMesh")
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


# Class to create the main dialog window for the FlowFinity Crowd Simulation tool
class FlowFinityGUI(QtWidgets.QDialog):

    # Variable to track current window instance
    dlg_instance = None

    # Class method to show the dialog window if it is hidden, otherwise bring it to the front
    @classmethod
    def show_dialog(cls):
        if not cls.dlg_instance:
            cls.dlg_instance = FlowFinityGUI()
        cls.dlg_instance.show()

        if cls.dlg_instance.isHidden():
            cls.dlg_instance.show()
        else:
            cls.dlg_instance.raise_()
            cls.dlg_instance.activateWindow()

    def __init__(self, parent=maya_main_window()):
        super(FlowFinityGUI, self).__init__(parent)

        self.setWindowTitle("FlowFinity Crowd Simulation")
        self.setMinimumSize(300, 450)
        self.setWindowFlag(QtCore.Qt.WindowContextHelpButtonHint, False)

        self.create_widgets()
        self.create_layout()
        self.create_connections()

    def create_widgets(self):
        # Widgets for navigation mesh selection
        self.nav_mesh_label = QtWidgets.QLabel("Navigation Mesh:")
        self.nav_mesh_label.setStyleSheet("font-size: 12px;")
        self.search_bar = QtWidgets.QLineEdit()
        self.select_btn = QtWidgets.QPushButton()
        self.select_btn.setIcon(QtGui.QIcon(":search.png"))
        self.select_btn.setToolTip("Select File")

        # Widgets for path locator list and corresponding label
        self.path_label = QtWidgets.QLabel("Path Locators:")
        self.path_label.setStyleSheet("font-size: 12px;")
        self.path_list_widget = QtWidgets.QListWidget(self)
        self.path_list_widget.setMaximumHeight(100)
        self.path_list_widget.setStyleSheet("font-size: 12px;")

        # Buttons for adding and removing paths
        self.add_button = QtWidgets.QPushButton()
        self.add_button.setIcon(QtGui.QIcon(":addClip.png"))
        self.remove_button = QtWidgets.QPushButton()
        self.remove_button.setIcon(QtGui.QIcon(":delete.png"))

        # Widgets for agent spawning and behavior
        self.spawn_rate = QtWidgets.QSpinBox()
        self.spawn_rate.setRange(1, 100)
        self.spawn_rate.setValue(5)
        self.spawn_rate.setSuffix(" agents per second")
        self.spawn_rate.setSingleStep(1)
        self.spawn_rate.setMinimumHeight(30)

        self.max_agents = QtWidgets.QSpinBox()
        self.max_agents.setRange(1, 400)
        self.max_agents.setValue(200)
        self.max_agents.setSuffix(" agents")
        self.max_agents.setSingleStep(1)
        self.max_agents.setMinimumHeight(30)

        self.agent_velocity = QtWidgets.QDoubleSpinBox()
        self.agent_velocity.setRange(0.1, 10.0)
        self.agent_velocity.setValue(1.0)
        self.agent_velocity.setSuffix(" m/s")
        self.agent_velocity.setSingleStep(0.1)
        self.agent_velocity.setMinimumHeight(30)

        self.agent_acceleration = QtWidgets.QDoubleSpinBox()
        self.agent_acceleration.setRange(0.1, 10.0)
        self.agent_acceleration.setValue(1.0)
        self.agent_acceleration.setSuffix(" m/s^2")
        self.agent_acceleration.setSingleStep(0.1)
        self.agent_acceleration.setMinimumHeight(30)

        # Buttons for applying and closing the dialog at the bottom
        self.apply_btn = QtWidgets.QPushButton("Apply")
        self.create_btn = QtWidgets.QPushButton("Create")
        self.close_btn = QtWidgets.QPushButton("Close")

    def create_layout(self):
        nav_mesh_layout = QtWidgets.QHBoxLayout()
        nav_mesh_layout.addWidget(self.nav_mesh_label)
        nav_mesh_layout.addWidget(self.search_bar)
        nav_mesh_layout.addWidget(self.select_btn)

        path_button_layout = QtWidgets.QHBoxLayout()
        path_button_layout.addWidget(self.add_button)
        path_button_layout.addWidget(self.remove_button)

        list_layout = QtWidgets.QVBoxLayout()
        list_layout.addWidget(self.path_label)
        list_layout.addWidget(self.path_list_widget)
        list_layout.addLayout(path_button_layout)
        path_button_layout.setAlignment(QtCore.Qt.AlignLeft)
        path_button_layout.addStretch()
        list_layout.addStretch()

        button_layout = QtWidgets.QHBoxLayout()
        button_layout.addStretch()
        button_layout.addWidget(self.close_btn)
        button_layout.addWidget(self.apply_btn)
        button_layout.addWidget(self.create_btn)

        # Collapsible layout boxes for agent spawning and behavior

        collapsible_layout = QtWidgets.QVBoxLayout()
        agent_spawning_menu = CollapsibleBox("Agent Spawning".format())
        collapsible_layout.addWidget(agent_spawning_menu)
        agent_spawning_layout = QtWidgets.QFormLayout()
        agent_spawning_layout.addRow("Spawn Rate:", self.spawn_rate)
        agent_spawning_layout.addRow("Max Agents:", self.max_agents)

        # Sets the height of each row of the form layout to 30 (same as the spin box height)
        for i in range(agent_spawning_layout.rowCount()):
            item = agent_spawning_layout.itemAt(i)
            item.widget().setMinimumHeight(30)

        # Must be called after the widgets are added to the box layout to properly configure the collapsible box
        agent_spawning_menu.setContentLayout(agent_spawning_layout)

        agent_behavior_menu = CollapsibleBox("Agent Behavior".format())
        collapsible_layout.addWidget(agent_behavior_menu)
        agent_behavior_layout = QtWidgets.QFormLayout()
        agent_behavior_layout.addRow("Base Velocity:", self.agent_velocity)
        agent_behavior_layout.addRow("Acceleration:", self.agent_acceleration)

        for i in range(agent_behavior_layout.rowCount()):
            item = agent_behavior_layout.itemAt(i)
            item.widget().setMinimumHeight(30)

        agent_behavior_menu.setContentLayout(agent_behavior_layout)

        collapsible_layout.addStretch()

        main_layout = QtWidgets.QVBoxLayout(self)
        main_layout.addLayout(nav_mesh_layout)
        main_layout.addLayout(list_layout)
        main_layout.addLayout(collapsible_layout)
        main_layout.addStretch(1)
        main_layout.addLayout(button_layout)

    def create_connections(self):
        self.select_btn.clicked.connect(self.show_search_dialog)
        self.add_button.clicked.connect(self.add_path)
        self.remove_button.clicked.connect(self.remove_path)
        self.create_btn.clicked.connect(self.create_flowfinity)
        self.apply_btn.clicked.connect(self.run_flowfinity)
        self.close_btn.clicked.connect(self.close)

    # Shows the search dialog to select a navigation mesh
    def show_search_dialog(self):
        self.search_dialog = SearchNavMeshDialog(self)
        self.populate_mesh_list()
        self.search_dialog.exec_()

    # Populates the QListWidget in the search dialog with all meshes in the scene
    def populate_mesh_list(self):
        self.search_dialog.results_list.clear()
        scene_objects = cmds.ls(type="mesh")
        for obj in scene_objects:
            self.search_dialog.results_list.addItem(obj)

    # Adds a path to the path list widget as a string of the form "start_locator -> end_locator" if it is not already present
    def add_path(self):
        dialog = LocatorSelectionDialog(self)
        if dialog.exec_():
            selected_items = dialog.locator_list.selectedItems()
            start_locator = selected_items[0].text()
            end_locator = selected_items[1].text()
            path = f"{start_locator} -> {end_locator}"
            if path not in [
                self.path_list_widget.item(i).text()
                for i in range(self.path_list_widget.count())
            ]:
                self.path_list_widget.addItem(path)

    # Removes the selected path from the path list widget
    def remove_path(self):
        for item in self.path_list_widget.selectedItems():
            self.path_list_widget.takeItem(self.path_list_widget.row(item))

    def run_flowfinity(self):
        print("TODO: Implement run_flowfinity")

    def create_flowfinity(self):
        print("TODO: Implement create_flowfinity")


if __name__ == "__main__":
    # Close the existing dialog to avoid multiple instances
    try:
        flowfinity_gui.close()  # pylint: disable=E0601
        flowfinity_gui.deleteLater()
    except:
        pass

    flowfinity_gui = FlowFinityGUI()
    flowfinity_gui.show()
