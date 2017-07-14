using System.Collections;
using System.Collections.Generic;
using UnityEngine;
#if UNITY_EDITOR
using UnityEditor;
#endif

using JInput;

public class CloseOnTwoPresses : MonoBehaviour {

    public ControllerButtons m_Button;

    // Update is called once per frame
    void Update() {
        if (InputManager.numberOfControllers >= 2) {
            int numPressed = 0;
            for (int i = 0; i < InputManager.numberOfControllers; i++) {
                if (InputManager.getController(i).IsButtonDown(m_Button)) {
                    numPressed++;
                }
            }
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
