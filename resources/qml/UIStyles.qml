import QtQuick 1.1
import com.nokia.meego 1.1

QtObject {
    // Font properties
    property string fontFamily: "Nokia Pure Text"
    property int fontSizeXLarge: 32
    property int fontSizeLarge: 28
    property int fontSizeSLarge: 26
    property int fontSizeDefault: 24
    property int fontSizeLSmall: 22
    property int fontSizeSmall: 20
    property int fontSizeXSmall: 18
    property int fontSizeXXSmall: 16

    // Colors for foreground and background
    property color textColorPrimary: theme.inverted ? "#ffffff" : "#191919" // Primary text color
    property color textColorSecondary: theme.inverted ? "#8c8c8c" : "#8c8c8c" // Secondary text color
    property color backgroundColor: theme.inverted ? "#000000" : "#E0E1E2" // Background color
    property color selectionColor: "#4591ff" // Selected item background color

    // Disabled colors
    property color disabledTextColor: "#b2b2b4"

    // Button colors
    property color buttonTextColorPrimary: theme.inverted ? "#ffffff" : "#000000" // Text color for buttons
    property color buttonTextColorSecondary: "#8c8c8c" // Secondary text for buttons
    property color buttonTextColorDisabled: "#B2B2B4" // Disabled button text
    property color buttonBackgroundColor: "#000000" // Button background

    // Accent colors
    property variant accentColors: [
        "#FF0000", // 0: Red
        "#FFA500", // 1: Orange
        "#800080", // 2: Purple/Violet
        "#008000", // 3: Green
        "#00FFFF", // 4: Cyan
        "#0000FF", // 5: Blue
        "#FFC0CB"  // 6: Pink
    ]

    // Icon sizes
    property int iconSizeDefault: 32
    property int iconSizeLarge: 48

    // Corner margin and layout properties
    property int cornerMargin: 22
    property int marginDefault: 0
    property int marginXlarge: 16
    property int marginDefaultLayout: 16
    property int buttonSpacing: 6

    // Padding properties
    property int paddingXSmall: 2
    property int paddingSmall: 4
    property int paddingMedium: 6
    property int paddingLarge: 8
    property int paddingDouble: 12
    property int paddingXLarge: 16
    property int paddingXXLarge: 24

    // Button dimensions
    property int buttonSize: 64
    property int buttonWidth: 322
    property int buttonHeight: 51
    property int buttonLabelMargin: 10

    // Field and list item dimensions
    property int fieldHeightDefault: 52
    property int listItemHeightSmall: 64
    property int listItemHeightDefault: 80

    // Header layout properties (portrait and landscape)
    property int headerHeightPortrait: 72
    property int headerHeightLandscape: 46
    property int headerTopSpacingPortrait: 20
    property int headerBottomSpacingPortrait: 20
    property int headerTopSpacingLandscape: 16
    property int headerBottomSpacingLandscape: 14

    // Scroll decorator margins
    property int scrollDecoratorMarginShort: 8
    property int scrollDecoratorMarginLong: 4

    // Touch and release margins
    property int touchExpansionMargin: -4
    property int releaseMissDelta: 30

    // Opacity settings
    property real opacityEnabled: 1.0
    property real opacityDisabled: 0.5
}
