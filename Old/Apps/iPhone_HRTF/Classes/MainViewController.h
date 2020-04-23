//
//  MainViewController.h -- CSL_iphone_test
//
//  Created by charlie on 8/20/09.
//  Copyright One More Muse 2009. All rights reserved.
//

#import "FlipsideViewController.h"
#import "CSL_Graph.h"

@interface MainViewController : UIViewController <FlipsideViewControllerDelegate> {

	IBOutlet UITextField *textField;
	IBOutlet UILabel *label;
	NSString *string;
	CSL_Graph * csl;
}

@property (nonatomic, retain) UITextField *textField;
@property (nonatomic, retain) UILabel *label;
@property (nonatomic, copy) NSString *string;

- (IBAction)showInfo;

- (void)updateString;

@end
