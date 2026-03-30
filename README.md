# ATM-C-language
This project is a console-based ATM (Automated Teller Machine) simulation implemented in C.
It models basic banking operations such as account management, withdrawals, deposits, transfers, and transaction logging using binary files.  
The system is designed with a focus on low-level data handling (file I/O, structs, memory) .
Features
🔐 User Authentication
Login using Account Number (AL) and PIN
Limited login attempts
💰 Account Operations
Balance inquiry
Deposit (with banknote validation)
Withdrawal (with ATM cash constraints)
Money transfer between accounts
📜 Transaction System
Stores all transactions in a binary file
Displays last 4 transactions
Tracks daily withdrawals (limit: 600€)
🧾 Payments
Rent payments
Utility expenses
🔑 Security
PIN change functionality
Admin (hidden) functions for ATM control
🏦 ATM Cash Management
Tracks available banknotes (20€, 10€, 5€)
Prevents withdrawals if exact amount cannot be formed
🗂️ File Structure
accounts → Binary file storing all account data
transactions → Binary file storing transaction history
🧠 Core Concepts Used
Structs (struct log, struct trns)
Binary file handling (fread, fwrite)
Searching (Binary Search)
Pointers and memory handling
Date/time handling (time.h)
State persistence via files

How to Run
Compile the program:
gcc atm.c -o atm
Run:
./atm

the pin and password have the same value to login:  3152  3153  3154  3156  3159  3160  3161  3163  3164  3165  3166  3167  3168  3169  3170  3171  3174  3175  3177  3178  3179  3181  3183  3184  3185  3186  3187  3189  3192  3193  3196  3198  3200  3201  3203  3204  3205  3206  3209  3211  3212  3216  3217  3218  3220  3222  3230  3231  3232  3234  3235  3236  3237  3238  3239  3240  3241  3242  3244  3245  3246  3247  3248  3249  3250  3255  3257  3258  3259  3260  3261  3262  3263  3266  3267  3268  3271  3272  3274  3275  3276  3279  3281  3286  3287  3288  3290  3291  3293  3297  3298  3300  3303  3304  3305  3306  3307  3308  3309  3310  3311  3312  3313  3315  3316  3318  3319  3320  3321  3322  3324  3325  3326  3327  3328  3329  3330  3332  3333  3334  3335  3336  3338  3339  3340  3341  3342  3343  3344  3346  3347  3349  3350  3353  3353  3355  3356  3357  3358  3359  3361  3362  3363  3364  3367  3368  3369  3370  3371  3373  3376  3377  3378  3380  3381  3382  3383  3384  3385  3386  3387  3389  3390  3391  3393  3394  3396  3398  3399  3400  3402  3404  3406  3407  3410  3411  3412  3413  3414  3415  3416  3417  3418  3419  3420  3422  3423  3424  3425  3426  3428  3429  3430  3431  3433  3434  3435  3436  3438  3441  3442  3443  3445  3448  3449  3450  3451  3452  3453  3454  3455  3456  3459  3459  3460  3461  3462  3463  3464  3465  3466  3473  3474  3476  3477  3478  3480  3481  3483  3485  3486  3487  3488  3489  3492  3494  3495  3497  3498  3500  3501  3503  3505  3506  3507  3508  3510  3511  3512  3513  3516  3520  3522  3523  3525  3526  3527  3529  3530  3531  3534  3535  3536  3537  3539  3540  3543  3545  3546  3547  3548  3548  3549  3550  3553  3556  3557  3559  3560  3561  3562  3563  3566  3568  3569  3570  3571  3572  3573  3575  3576  3577  3578  3579  3580  3581  3582  3585  3586  3587  3588  3591  3592  3593  3594  3597  3598  3599  3600  3601  3602  3603  3604  3605  3607  3608  3609  3610  3612  3613  3614  3618  3619  3620  3621  3623  3624  3626  3629  3630  3631  3634  3635  3636  3637  3639  3641  3643  3644  3645  3646  3650  3652  3654  3655  3656  3657  3659  3660  3661  3663  3665  3666  3667  3668  3669  3670  3671  3673  3674  3675  3679  3680  3683  3685  3687  3688  3690  3691  3694  3695  3696  3697  3701  3702  3703  3704  3705  3706  3707  3708  3710  3711  3712  3713  3714  3717  3718  3721  3722  3723  3726  3727  3728  3730  3732  3733  3734  3736  3737  3738  3738  3740  3742  3743  3744  3745  3746  3747  3750  3752  3754  3755  3757  3758  3759  3761  3764  3765  3766  3767  3769  3771  3772  3773  3774  3776  3777  3778  3779  3780  3781  3782  3785  3786  3788  3789  3790  3791  3792  3794  3796  3799  3800  3802  3803  3804  3805  3806  3807  3808  3811  3813  3814  3815  3816  3817  3818  3819  3820  3824  3825  3827  3829  3830  3831  3832  3835  3836  3837  3838  3839  3840



⚠️ Make sure the .exe and the transactions,accounts file exist in the same folder  
🔒 Admin Access

Hidden admin options: here are 

101 → View ATM cash
102 → Refill ATM
103 → Shutdown system

Credentials:

Username: PELEKAS
PIN: 5425
📈 Possible Improvements
Replace binary files with a database
Add encryption for sensitive data
GUI interface
📚 Purpose

This project is intended for educational use, demonstrating how low-level systems like ATMs can be simulated using C and fundamental programming concepts.
