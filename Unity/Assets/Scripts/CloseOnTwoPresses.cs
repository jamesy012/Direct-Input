using System.Collections;
using System.Collections.Generic;
using UnityEngine;
#if UNITY_EDITOR
using UnityEditor;
#endif


using Input = JInput.JInputManager;

public class CloseOnTwoPresses : MonoBehaviour {

    public JInput.ControllerButtons m_Button;

    // Update is called once per frame
    void Update() {
        if (Input.m_Input.numberOfControllers >= 2) {
            int numPressed = 0;
            for (int i = 0; i < Input.m_Input.numberOfControllers; i++) {
                if (Input.m_Input.getController(i).IsButtonDown(m_Button)) {
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
