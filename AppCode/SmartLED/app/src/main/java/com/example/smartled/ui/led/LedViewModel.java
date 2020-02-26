package com.example.smartled.ui.led;

import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

public class LedViewModel extends ViewModel {

    private MutableLiveData<String> mText;

    public LedViewModel() {
        mText = new MutableLiveData<>();
        mText.setValue("This is led fragment");
    }

    public LiveData<String> getText() {
        return mText;
    }
}