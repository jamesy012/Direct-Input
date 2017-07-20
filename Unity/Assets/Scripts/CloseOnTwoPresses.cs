using System.Collections;
using System.Collections.Generic;
using UnityEngine;
#if UNITY_EDITOR
using UnityEditor;
#endif

//removes JInput prefix from all input calls
using JInput;

public class CloseOnTwoPresses : MonoBehaviour {

    //which button should we be looking at
    public ControllerButtons m_Button;

    // Update is called once per frame
    void Update() {
        //check to see if there are more then two controllers
        if (InputManager.numberOfControllers >= 2) {
            //counter for how many button presses there are
            int numPressed = 0;
            //loop through all controllers
            for (int i = 0; i < InputManager.numberOfControllers; i++) {
                //check to see if m_BUtton is down
                if (InputManager.getController(i).IsButtonDown(m_Button)) {
                    //increment
                    numPressed++;
                }
            }

            //if there were two or more buttons down
            //then pause in editor or quit
            if (numPressed >= 2) {
#if UNITY_EDITOR
                EditorApplication.isPaused = true;
#else
                Application.Quit();
#endif
            }
        }
    }
}
