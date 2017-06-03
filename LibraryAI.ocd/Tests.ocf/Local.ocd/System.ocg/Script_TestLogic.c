
/*-- Test Control --*/

// Aborts the current test and launches the specified test instead.
global func LaunchTest(int nr)
{
	// Get the control test.
	var test = CurrentTest();
	if (test)
	{
		// Finish the currently running test.
		Call(Format("~Test%d_OnFinished", test.testnr));
		// Start the requested test by just setting the test number and setting 
		// test.launched to false, effect will handle the rest.
	}
	else
	{
		// Create a new control effect and launch the test.
		test = AddEffect("IntTestControl", nil, 100, 1);
		test.player = GetPlayerByIndex(0, C4PT_User);
		test.global_result = true;
		test.count_total = 0;
		test.count_failed = 0;
		test.count_skipped = 0;
	}

	test.testnr = nr;
	test.launched = false;
}

// Calling this function skips the current test, does not work if last test has been ran already.
global func SkipTest()
{
	// Get the control test.
	var test = CurrentTest();
	if (test)
	{
		// Finish the previous test.
		Call(Format("~Test%d_OnFinished", test.testnr));
		// Start the next test by just increasing the test number and setting 
		// test.launched to false, effect will handle the rest.
		test.testnr++;
		test.launched = false;
		test.count_skipped++;
	}
}


/*-- Test Effect --*/

global func FxIntTestControlStart(object target, proplist test, int temporary)
{
	if (temporary)
		return FX_OK;
	// Set default interval.
	test.Interval = 1;
	return FX_OK;
}

global func FxIntTestControlTimer(object target, proplist test)
{
	// Launch new test if needed.
	if (!test.launched)
	{
		// Log test start.
		Log("=====================================");
		Log("Test %d started:", test.testnr);
		// Start the test if available, otherwise finish test sequence.
		if (!Call(Format("~Test%d_OnStart", test.testnr), test.player))
		{
			Log("Test %d not available, the previous test was the last test.", test.testnr);
			Log("=====================================");
			Log("All tests have been completed!");
			Log("* %d tests total", test.count_total);
			Log("%d tests failed", test.count_failed);
			Log("%d tests skipped", test.count_skipped);
			Log("=====================================");
			if (test.count_skipped == 0 && test.count_failed == 0 && test.count_total > 0)
			{
				Log("All tests passed!");
			}
			else
			{
				Log("At least one test failed or was skipped!");
			}
			return FX_Execute_Kill;
		}
		test.launched = true;
		test.count_total++;
	}
	
	// waiting
	if (test.wait > 0)
	{
		test.wait -= 1;
		return FX_OK;
	}
	
	// Check whether the current test has been finished.
	if (Call(Format("Test%d_Execute", test.testnr)))
	{
		test.launched = false;
		
		if (test.current_result)
		{
			Log(">> Test %d passed.", test.testnr);
		}
		else
		{
			Log(">> Test %d failed.", test.testnr);
			test.count_failed++;
		}
		
		// Update global result
		test.global_result &= test.current_result;
		test.current_result = false;

		// Call the test on finished function.
		Call(Format("~Test%d_OnFinished", test.testnr));
		// Log result and increase test number.
		Log("Test %d successfully completed.", test.testnr);
		test.testnr++;
	}
	return FX_OK;
}


global func doTest(description, returned, expected)
{
	var test = (returned == expected);
	
	var predicate = "[Fail]";
	if (test) predicate = "[Pass]";
	
	Log(Format("%s %s", predicate, description), returned, expected);
	return test;
}

global func CurrentTest()
{
	return GetEffect("IntTestControl", nil);
}

global func PassTest()
{
	CurrentTest().current_result = true;
	return true;
}

global func FailTest()
{
	CurrentTest().current_result = false;
	return true;
}

global func Wait(int amount)
{
	CurrentTest().wait = Max(0, amount);
	return false;
}
