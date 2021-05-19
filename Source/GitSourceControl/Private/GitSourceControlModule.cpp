// Copyright (c) 2014-2020 Sebastien Rombauts (sebastien.rombauts@gmail.com)
//
// Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
// or copy at http://opensource.org/licenses/MIT)

#include "GitSourceControlModule.h"


#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "EditorStyleSet.h"
#include "Features/IModularFeatures.h"
#include "Framework/Commands/UIAction.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "GitSourceControlOperations.h"
#include "LevelEditor.h"
#include "SourceControlHelpers.h"
#include "SourceControlOperations.h"
#include "Misc/App.h"
#include "Modules/ModuleManager.h"
#include "Textures/SlateIcon.h"
#include "GitSourceControlUtils.h"

#define LOCTEXT_NAMESPACE "GitSourceControl"

template < typename Type >
static TSharedRef< IGitSourceControlWorker, ESPMode::ThreadSafe > CreateWorker()
{
    return MakeShareable( new Type() );
}

FText MenuValidateDataGetTitle()
{
    return LOCTEXT( "NamingConventionValidationTitle", "Naming Convention..." );
}

void DiffAgainstOriginDevelop( UObject* InObject, const FString& InPackagePath, const FString& InPackageName )
{
	check( InObject );

	const FGitSourceControlModule& GitSourceControl = FModuleManager::GetModuleChecked<FGitSourceControlModule>( "GitSourceControl" );
	const auto PathToGitBinary = GitSourceControl.AccessSettings().GetBinaryPath();
	const auto PathToRepositoryRoot = GitSourceControl.GetProvider().GetPathToRepositoryRoot();

    ISourceControlProvider& SourceControlProvider = ISourceControlModule::Get().GetProvider();
	
	FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");

	// Get the SCC state
	FSourceControlStatePtr SourceControlState = SourceControlProvider.GetState(SourceControlHelpers::PackageFilename(InPackagePath), EStateCacheUsage::Use);

	// If we have an asset and its in SCC..
	if( SourceControlState.IsValid() && InObject != nullptr && SourceControlState->IsSourceControlled() )
	{
		// Get the file name of package
		FString RelativeFileName;
		if(FPackageName::DoesPackageExist(InPackagePath, nullptr, &RelativeFileName))
		{
			//if(SourceControlState->GetHistorySize() > 0)
			{
				TArray< FString > Errors;
                const auto Revision = GitSourceControlUtils::GetOriginDevelopRevision( PathToGitBinary, PathToRepositoryRoot, RelativeFileName, Errors );

	            check(Revision.IsValid());

				FString TempFileName;
				if(Revision->Get(TempFileName))
				{
					// Try and load that package
					UPackage* TempPackage = LoadPackage(nullptr, *TempFileName, LOAD_ForDiff|LOAD_DisableCompileOnLoad);
					if(TempPackage != nullptr)
					{
						// Grab the old asset from that old package
						UObject* OldObject = FindObject<UObject>(TempPackage, *InPackageName);
						if(OldObject != nullptr)
						{
							/* Set the revision information*/
							FRevisionInfo OldRevision;
							OldRevision.Changelist = Revision->GetCheckInIdentifier();
							OldRevision.Date = Revision->GetDate();
							OldRevision.Revision = Revision->GetRevision();

							FRevisionInfo NewRevision; 
							NewRevision.Revision = TEXT("");

							AssetToolsModule.Get().DiffAssets( OldObject, InObject, OldRevision, NewRevision);
						}
					}
				}
			}
		} 
	}
}

void DiffAssetAgainstGitOriginDevelop( const TArray< FAssetData > selected_assets )
{
    for(int32 AssetIdx=0; AssetIdx<selected_assets.Num(); AssetIdx++)
	{
		// Get the actual asset (will load it)
		const FAssetData& AssetData = selected_assets[AssetIdx];

		UObject* CurrentObject = AssetData.GetAsset();
		if( CurrentObject )
		{
			const FString PackagePath = AssetData.PackageName.ToString();
			const FString PackageName = AssetData.AssetName.ToString();
			DiffAgainstOriginDevelop( CurrentObject, PackagePath, PackageName );
		}
	}
}

void CreateGitContentBrowserAssetMenu( FMenuBuilder & menu_builder, const TArray< FAssetData > selected_assets )
{
    menu_builder.AddMenuEntry(
        LOCTEXT( "GitPlugin", "Diff against origin/develop" ),
        LOCTEXT( "GitPlugin", "Diff that asset against the version on origin/develop." ),
        FSlateIcon(FEditorStyle::GetStyleSetName(), "SourceControl.Actions.Diff"),
        FUIAction( FExecuteAction::CreateStatic( DiffAssetAgainstGitOriginDevelop, selected_assets ) ) );
}

TSharedRef< FExtender > OnExtendContentBrowserAssetSelectionMenu( const TArray< FAssetData > & selected_assets )
{
    TSharedRef< FExtender > extender( new FExtender() );

    extender->AddMenuExtension(
        "AssetSourceControlActions",
        EExtensionHook::After,
        nullptr,
        FMenuExtensionDelegate::CreateStatic( CreateGitContentBrowserAssetMenu, selected_assets ) );

    return extender;
}
void FGitSourceControlModule::StartupModule()
{
    // Register our operations (implemented in GitSourceControlOperations.cpp by subclassing from Engine\Source\Developer\SourceControl\Public\SourceControlOperations.h)
    GitSourceControlProvider.RegisterWorker( "Connect", FGetGitSourceControlWorker::CreateStatic( &CreateWorker< FGitConnectWorker > ) );
    // Note: this provider uses the "CheckOut" command only with Git LFS 2 "lock" command, since Git itself has no lock command (all tracked files in the working copy are always already checked-out).
    GitSourceControlProvider.RegisterWorker( "CheckOut", FGetGitSourceControlWorker::CreateStatic( &CreateWorker< FGitCheckOutWorker > ) );
    GitSourceControlProvider.RegisterWorker( "UpdateStatus", FGetGitSourceControlWorker::CreateStatic( &CreateWorker< FGitUpdateStatusWorker > ) );
    GitSourceControlProvider.RegisterWorker( "MarkForAdd", FGetGitSourceControlWorker::CreateStatic( &CreateWorker< FGitMarkForAddWorker > ) );
    GitSourceControlProvider.RegisterWorker( "Delete", FGetGitSourceControlWorker::CreateStatic( &CreateWorker< FGitDeleteWorker > ) );
    GitSourceControlProvider.RegisterWorker( "Revert", FGetGitSourceControlWorker::CreateStatic( &CreateWorker< FGitRevertWorker > ) );
    GitSourceControlProvider.RegisterWorker( "Sync", FGetGitSourceControlWorker::CreateStatic( &CreateWorker< FGitSyncWorker > ) );
    GitSourceControlProvider.RegisterWorker( "Push", FGetGitSourceControlWorker::CreateStatic( &CreateWorker< FGitPushWorker > ) );
    GitSourceControlProvider.RegisterWorker( "CheckIn", FGetGitSourceControlWorker::CreateStatic( &CreateWorker< FGitCheckInWorker > ) );
    GitSourceControlProvider.RegisterWorker( "Copy", FGetGitSourceControlWorker::CreateStatic( &CreateWorker< FGitCopyWorker > ) );
    GitSourceControlProvider.RegisterWorker( "Resolve", FGetGitSourceControlWorker::CreateStatic( &CreateWorker< FGitResolveWorker > ) );

    // load our settings
    GitSourceControlSettings.LoadSettings();

    // Bind our source control provider to the editor
    IModularFeatures::Get().RegisterModularFeature( "SourceControl", &GitSourceControlProvider );

    auto & content_browser_module = FModuleManager::LoadModuleChecked< FContentBrowserModule >( TEXT( "ContentBrowser" ) );
    auto & cb_asset_menu_extender_delegates = content_browser_module.GetAllAssetViewContextMenuExtenders();

    cb_asset_menu_extender_delegates.Add( FContentBrowserMenuExtender_SelectedAssets::CreateStatic( OnExtendContentBrowserAssetSelectionMenu ) );
    ContentBrowserAssetExtenderDelegateHandle = cb_asset_menu_extender_delegates.Last().GetHandle();
}

void FGitSourceControlModule::ShutdownModule()
{
    // shut down the provider, as this module is going away
    GitSourceControlProvider.Close();

    // unbind provider from editor
    IModularFeatures::Get().UnregisterModularFeature( "SourceControl", &GitSourceControlProvider );

    if ( auto * content_browser_module = FModuleManager::GetModulePtr< FContentBrowserModule >( TEXT( "ContentBrowser" ) ) )
    {
        auto & content_browser_menu_extender_delegates = content_browser_module->GetAllAssetViewContextMenuExtenders();
        content_browser_menu_extender_delegates.RemoveAll( [ &extender_delegate = ContentBrowserAssetExtenderDelegateHandle ]( const FContentBrowserMenuExtender_SelectedAssets & delegate ) {
            return delegate.GetHandle() == extender_delegate;
        } );
    }
}

void FGitSourceControlModule::SaveSettings()
{
    if ( FApp::IsUnattended() || IsRunningCommandlet() )
    {
        return;
    }

    GitSourceControlSettings.SaveSettings();
}

IMPLEMENT_MODULE( FGitSourceControlModule, GitSourceControl );

#undef LOCTEXT_NAMESPACE
