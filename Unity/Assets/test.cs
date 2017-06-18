using System.Collections;
using System.Collections.Generic;
using System;
using UnityEngine;
using System.Runtime.InteropServices;

[System.Serializable]
public class ControllerData {
    public float[] axesValues = new float[6];
    public bool[] buttons = new bool[16];
    public int hatSwitch = 0;
}

public enum ControllerAxes {
    LStickX,
    LStickY,
    RStickX,
    RStickY,
    LeftTrigger,
    RightTrigger
};

public class test : MonoBehaviour {

    [DllImport("UnityDirectInputDll")]
    private static extern int startInput();

    [DllImport("UnityDirectInputDll")]
    private static extern void releaseInput();

    [DllImport("UnityDirectInputDll")]
    private static extern int updateInput();

    [DllImport("UnityDirectInputDll")]
    private static extern int getButton(int a_Button);

    [DllImport("UnityDirectInputDll")]
    private static extern int getAxesValue(int a_Axis);

    [DllImport("UnityDirectInputDll")]
    private static extern int getPovValue();

    [DllImport("UnityDirectInputDll")]
    private static extern int getPovDir();

    [DllImport("UnityDirectInputDll")]
    private static extern IntPtr getPovName(int a_Direction);


    public static string getNameFromHat(int a_Hat) {
        return Marshal.PtrToStringAuto(getPovName(a_Hat));
    }


    public ControllerData m_Controller;

    //[DllImport("user32.dll")]
    //private static extern System.IntPtr GetActiveWindow();
    //
    //public static System.IntPtr GetWindowHandle() {
    //    return GetActiveWindow();
    //}

    // Use this for initialization
    void Start () {
        startInput();
    }
	
	// Update is called once per frame
	void Update () {
        updateInput();

        updateController(m_Controller);
	}

    void updateController(ControllerData a_Controller) {
        for (int i = 0; i < 16; i++) {
            a_Controller.buttons[i] = getButton(i) >= 1;
        }
        for (int i = 0; i < 6; i++) {
            a_Controller.axesValues[i] = getAxesValue(i);
        }
        a_Controller.hatSwitch = getPovDir();
    }
}
